import argparse
import logging
import os
import re
import sys

_log = logging.getLogger('lwl')
_log_handler = logging.StreamHandler(sys.stdout)
_log.addHandler(_log_handler)

class LwlMsg:
    """
    Represents metadata for a single log message.
    Extracted from lwl_msg_table in source files to format raw log data.
    """
    def __init__(self, id, fmt, num_arg_bytes, arg_sizes, file_path, line_num):
        self.id = id
        self.fmt = fmt
        self.num_arg_bytes = num_arg_bytes
        self.arg_sizes = arg_sizes
        self.file_path = file_path
        self.line_num = line_num
        _log.debug('Create LwlMsg ID=%d num_arg_bytes=%d arg_sizes=%s for %s:%d' %
                   (id, num_arg_bytes, arg_sizes, file_path, line_num))

class LwlMsgSet:
    """
    Manages the set of log message metadata.
    Ensures consistency and stores the maximum message length.
    """
    def __init__(self):
        self.lwl_msgs = {}
        self.max_msg_len = 0

    def add_lwl_msg(self, id, fmt, num_arg_bytes, arg_sizes, file_path, line_num):
        if id in self.lwl_msgs:
            print('%s:%d Duplicate ID %d: %s, previously at %s:%d' %
                  (file_path, line_num, id, fmt,
                   self.lwl_msgs[id].file_path,
                   self.lwl_msgs[id].line_num))
            return False
        self.lwl_msgs[id] = LwlMsg(id, fmt, num_arg_bytes, arg_sizes, file_path, line_num)
        msg_len = 3 + num_arg_bytes + 1  # START_BYTE + LENGTH + ID + ARGS + CRC
        if msg_len > self.max_msg_len:
            self.max_msg_len = msg_len
            _log.debug('New max msg len %d for %s:%d %s' %
                       (msg_len, file_path, line_num, fmt))
        return True

    def get_metadata(self, id):
        try:
            _log.debug('get_metadata(id=%d): Found in lwl_msgs', id)
            return self.lwl_msgs[id]
        except KeyError:
            _log.debug('get_metadata(id=%d): Not found in lwl_msgs', id)
            return None

lwl_msg_set = LwlMsgSet()

# CRC-8-ATM table (polynomial 0x07)
crc8_table = [
    0x00, 0x07, 0x0E, 0x09, 0x1C, 0x1B, 0x12, 0x15, 0x38, 0x3F, 0x36, 0x31, 0x24, 0x23, 0x2A, 0x2D,
    0x70, 0x77, 0x7E, 0x79, 0x6C, 0x6B, 0x62, 0x65, 0x48, 0x4F, 0x46, 0x41, 0x54, 0x53, 0x5A, 0x5D,
    0xE0, 0xE7, 0xEE, 0xE9, 0xFC, 0xFB, 0xF2, 0xF5, 0xD8, 0xDF, 0xD6, 0xD1, 0xC4, 0xC3, 0xCA, 0xCD,
    0x90, 0x97, 0x9E, 0x99, 0x8C, 0x8B, 0x82, 0x85, 0xA8, 0xAF, 0xA6, 0xA1, 0xB4, 0xB3, 0xBA, 0xBD,
    0xC7, 0xC0, 0xC9, 0xCE, 0xDB, 0xDC, 0xD5, 0xD2, 0xFF, 0xF8, 0xF1, 0xF6, 0xE3, 0xE4, 0xED, 0xEA,
    0xB7, 0xB0, 0xB9, 0xBE, 0xAB, 0xAC, 0xA5, 0xA2, 0x8F, 0x88, 0x81, 0x86, 0x93, 0x94, 0x9D, 0x9A,
    0x27, 0x20, 0x29, 0x2E, 0x3B, 0x3C, 0x35, 0x32, 0x1F, 0x18, 0x11, 0x16, 0x03, 0x04, 0x0D, 0x0A,
    0x57, 0x50, 0x59, 0x5E, 0x4B, 0x4C, 0x45, 0x42, 0x6F, 0x68, 0x61, 0x66, 0x73, 0x74, 0x7D, 0x7A,
    0x89, 0x8E, 0x87, 0x80, 0x95, 0x92, 0x9B, 0x9C, 0xB1, 0xB6, 0xBF, 0xB8, 0xAD, 0xAA, 0xA3, 0xA4,
    0xF9, 0xFE, 0xF7, 0xF0, 0xE5, 0xE2, 0xEB, 0xEC, 0xC1, 0xC6, 0xCF, 0xC8, 0xDD, 0xDA, 0xD3, 0xD4,
    0x69, 0x6E, 0x67, 0x60, 0x75, 0x72, 0x7B, 0x7C, 0x51, 0x56, 0x5F, 0x58, 0x4D, 0x4A, 0x43, 0x44,
    0x19, 0x1E, 0x17, 0x10, 0x05, 0x02, 0x0B, 0x0C, 0x21, 0x26, 0x2F, 0x28, 0x3D, 0x3A, 0x33, 0x34,
    0x4E, 0x49, 0x40, 0x47, 0x52, 0x55, 0x5C, 0x5B, 0x76, 0x71, 0x78, 0x7F, 0x6A, 0x6D, 0x64, 0x63,
    0x3E, 0x39, 0x30, 0x37, 0x22, 0x25, 0x2C, 0x2B, 0x06, 0x01, 0x08, 0x0F, 0x1A, 0x1D, 0x14, 0x13,
    0xAE, 0xA9, 0xA0, 0xA7, 0xB2, 0xB5, 0xBC, 0xBB, 0x96, 0x91, 0x98, 0x9F, 0x8A, 0x8D, 0x84, 0x83,
    0xDE, 0xD9, 0xD0, 0xD7, 0xC2, 0xC5, 0xCC, 0xCB, 0xE6, 0xE1, 0xE8, 0xEF, 0xFA, 0xFD, 0xF4, 0xF3
]

def calculate_crc8(data):
    crc = 0x00  # Initial value
    for byte in data:
        crc = crc8_table[crc ^ byte]
    return crc

def get_num_fmt_params(fmt):
    """Returns the number of parameters and their sizes from a format string."""
    arg_sizes = []
    state = 'idle'
    for c in fmt:
        if state == 'idle':
            if c == '%':
                state = 'got_percent'
            continue
        if state == 'got_percent':
            if c == '%':
                state = 'idle'
            elif c in '124':
                state = 'got_size'
                size = int(c)
            else:  # Handle plain %d as 4 bytes
                state = 'got_size'
                size = 4  # Default size for %d
            continue
        if state == 'got_size':
            if c == 'd':
                arg_sizes.append(size)
                state = 'idle'
            continue
    _log.debug('get_num_fmt_params(%s) returns %d params, sizes=%s', fmt, len(arg_sizes), arg_sizes)
    return len(arg_sizes), arg_sizes

def parse_source_file(file_path):
    """
    Parse a C source file to find lwl_msg_table definitions.
    Example: {NULL, 0}, // ID 0
             {"Timestamp %4d", 4},
    """
    error_count = 0
    _log.debug('parse_source_file(file_path=%s)', file_path)
    table_start_pat = re.compile(r'^\s*static\s+const\s+struct\s+lwl_msg\s+lwl_msg_table\[\]\s*=\s*{')
    table_entry_pat = re.compile(r'^\s*{\s*("[^"]*"|NULL)\s*,\s*(\d+)\s*}\s*,?\s*(//.*)?$')
    
    try:
        with open(file_path, encoding='utf-8') as f:
            lines = f.readlines()
    except FileNotFoundError:
        print(f'ERROR: Source file {file_path} not found')
        return 1
    except IOError:
        print(f'ERROR: Failed to read source file {file_path}')
        return 1

    line_num = 0
    in_table = False
    table_index = 0
    for line in lines:
        line_num += 1
        line = line.strip()
        if not line:
            continue

        # Detect start of lwl_msg_table
        if re.match(table_start_pat, line):
            in_table = True
            _log.debug('%s:%d Found lwl_msg_table start', file_path, line_num)
            table_index = 0
            continue

        # Parse table entries
        if in_table:
            _log.debug('Checking line %d: %s', line_num, line)
            m = re.match(table_entry_pat, line)
            if m:
                fmt = m.group(1).strip('"') if m.group(1) != 'NULL' else None
                num_arg_bytes = int(m.group(2))
                _log.debug('%s:%d Parsed entry: fmt=%s, num_arg_bytes=%d',
                           file_path, line_num, fmt, num_arg_bytes)
                num_params, arg_sizes = get_num_fmt_params(fmt) if fmt else (0, [])
                total_bytes = sum(arg_sizes)
                if fmt and total_bytes != num_arg_bytes:
                    print('ERROR: %s:%d Format string %s requires %d bytes but num_arg_bytes=%d' %
                          (file_path, line_num, fmt, total_bytes, num_arg_bytes))
                    error_count += 1
                else:
                    if not lwl_msg_set.add_lwl_msg(table_index, fmt, num_arg_bytes, arg_sizes, file_path, line_num):
                        error_count += 1
                table_index += 1
            elif '};' in line:
                in_table = False
                _log.debug('%s:%d Found lwl_msg_table end', file_path, line_num)
    _log.debug('parse_source_file(%s) completed, lwl_msgs=%s', file_path, lwl_msg_set.lwl_msgs)
    return error_count

def get_data_bytes(data, idx, num_bytes):
    """
    Extract a value (num_bytes) from the data buffer and advance the index in little-endian order.
    """
    _log.debug('get_data_bytes(idx=%d, num_bytes=%d)' % (idx, num_bytes))
    if idx + num_bytes > len(data):
        raise EOFError
    d = 0
    for i in range(num_bytes):
        d += data[idx + i] << (8 * i)
    idx += num_bytes
    _log.debug('get_data_bytes returns (d=%d, idx=%d)', d, idx)
    return d, idx

def decode_log_data(data, output_file='output.txt'):
    """
    Decode and write log messages from the binary data buffer to a file.
    Format: [START_BYTE (0xAA)][LENGTH][ID][ARG_BYTES][CRC]
    LENGTH is the total length including LENGTH, ID, ARG_BYTES, and CRC.
    CRC is calculated over [ID][ARG_BYTES].
    """
    _log.debug('decode_log_data(output_file=%s)', output_file)
    idx = 0
    num_data_bytes = len(data)
    valid_records = 0
    invalid_records = 0
    output_lines = []

    while idx < num_data_bytes:
        if data[idx] != 0xAA:  # Start byte
            _log.debug('Skipped non-start byte 0x%02x at idx=%d', data[idx], idx)
            idx += 1
            continue
        start_idx = idx
        idx += 1
        if idx >= num_data_bytes:
            _log.debug('Incomplete record at idx=%d: missing length', start_idx)
            invalid_records += 1
            break
        try:
            # Read length
            length, idx = get_data_bytes(data, idx, 1)
            if idx + length > num_data_bytes:
                _log.debug('Invalid length %d at idx=%d: exceeds buffer size', length, start_idx)
                invalid_records += 1
                break
            # Read ID
            id, idx = get_data_bytes(data, idx, 1)
            msg_meta = lwl_msg_set.get_metadata(id)
            if msg_meta is None:
                _log.debug('Invalid ID %d at idx=%d', id, start_idx)
                invalid_records += 1
                idx = start_idx + length + 1  # Skip to next record
                continue
            # Check length
            expected_length = 1 + 1 + msg_meta.num_arg_bytes + 1  # LENGTH + ID + ARGS + CRC
            if length != expected_length:
                _log.debug('Length mismatch for ID=%d at idx=%d: expected %d, got %d',
                           id, start_idx, expected_length, length)
                invalid_records += 1
                idx = start_idx + length + 1
                continue
            # Read arguments
            arg_values = []
            crc_data = [id]
            for size in msg_meta.arg_sizes:
                arg_value, idx = get_data_bytes(data, idx, size)
                arg_values.append(arg_value)
                # Collect bytes for CRC in little-endian order
                for i in range(size):
                    crc_data.append(data[idx - size + i])
            # Read CRC
            crc, idx = get_data_bytes(data, idx, 1)
            # Verify CRC
            calculated_crc = calculate_crc8(crc_data)
            if crc != calculated_crc:
                _log.debug('CRC mismatch for ID=%d at idx=%d: expected 0x%02x, got 0x%02x',
                           id, start_idx, calculated_crc, crc)
                invalid_records += 1
                continue
            # Format output
            if msg_meta.fmt:
                output_lines.append(msg_meta.fmt % tuple(arg_values) if arg_values else msg_meta.fmt)
            else:
                output_lines.append("NULL format string")
            _log.debug('Valid log ID=%d at idx=%d: %s', id, start_idx,
                       msg_meta.fmt % tuple(arg_values) if arg_values else msg_meta.fmt if msg_meta.fmt else "NULL")
            valid_records += 1
        except EOFError:
            _log.debug('Incomplete record at idx=%d', start_idx)
            invalid_records += 1
            break

    if invalid_records > 0 or idx < num_data_bytes:
        unused_data = data[idx:]
        if unused_data:
            output_lines.append('Unused/invalid data (hex): %s' % ' '.join(['%02x' % b for b in unused_data]))
    _log.info('Decoded %d valid records, %d invalid records', valid_records, invalid_records)

    try:
        with open(output_file, 'w', encoding='utf-8') as f:
            for line in output_lines:
                f.write(line + '\n')
    except IOError:
        print(f'ERROR: Failed to write to output file {output_file}')
        raise

def parse_source_dir(dir_path):
    """
    Parse all .c files in a directory to find lwl_msg_table definitions.
    """
    error_count = 0
    _log.debug('parse_source_dir(dir_path=%s)' % dir_path)
    for root_dir_path, _, file_names in os.walk(dir_path):
        for file_name in file_names:
            file_path = os.path.join(root_dir_path, file_name)
            if file_path[-2:] != '.c':
                _log.debug('Skip parsing for %s' % file_path)
                continue
            error_count += parse_source_file(file_path)
    return error_count

def decode_log_file(file_path, output_file='output.txt'):
    """
    Decode binary data from log.bin and write to output file.
    """
    _log.debug('decode_log_file(file_path=%s, output_file=%s)', file_path, output_file)
    try:
        with open(file_path, 'rb') as f:
            data = bytearray(f.read())
    except FileNotFoundError:
        print(f'ERROR: Log file {file_path} not found')
        return 1
    except IOError:
        print(f'ERROR: Failed to read log file {file_path}')
        return 1
    if not data:
        print('ERROR: Log file is empty')
        return 1
    _log.debug('Read %d bytes from log file', len(data))
    decode_log_data(data, output_file)
    return 0

def main():
    _log.setLevel(logging.DEBUG)  # Default log level is debug
    parser = argparse.ArgumentParser(description='Decode LWL log data from a binary file')
    parser.add_argument('-f', help='C source file to find lwl_msg_table definitions', action='append')
    parser.add_argument('-d', help='Directory containing C source files with lwl_msg_table', action='append')
    parser.add_argument('--log', help='Log level: notset|debug|info|warning|error|critical', default='debug')
    parser.add_argument('--bin', help='Binary log file to decode (default: log.bin)', default='obc_log_R_20250807_001541.bin')
    parser.add_argument('--output', help='Output file for decoded logs (default: output.txt)', default='output.txt')
    args = parser.parse_args()
    log_map = {
        'CRITICAL': logging.CRITICAL,
        'ERROR': logging.ERROR,
        'WARNING': logging.WARNING,
        'INFO': logging.INFO,
        'DEBUG': logging.DEBUG,
        'NOTSET': logging.NOTSET,
    }
    if args.log.upper() not in log_map:
        print('Invalid log level. Valid levels are (case does not matter):')
        for level in log_map:
            print('  %s' % level.lower())
        exit(1)
    _log.setLevel(log_map[args.log.upper()])
    error_count = 0
    default_files = ['./lwl.c']  # Default file path
    if args.d:
        for dir_path in args.d:
            error_count += parse_source_dir(dir_path)
    if args.f:
        for file_path in args.f:
            error_count += parse_source_file(file_path)
    elif not args.d:  # Use default file if neither -f nor -d is provided
        for file_path in default_files:
            error_count += parse_source_file(file_path)
    if error_count == 0:
        error_count += decode_log_file(args.bin, args.output)
    if error_count > 0:
        print(f'Completed with {error_count} errors')
        exit(1)

if __name__ == '__main__':
    try:
        main()
    except KeyboardInterrupt:
        print('Got keyboard interrupt')
        exit(1)