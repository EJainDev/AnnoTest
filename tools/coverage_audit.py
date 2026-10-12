"""Audit emitted runtime source decisions from GCC 16 gcov JSON.

Collapse template specializations of a single Boolean source condition. Retain
short-circuit subconditions, loop exits, catch dispatch and switch arms. Exclude
call/exception arcs and exception-only compiler cleanup blocks. Constant
evaluation and compile-time reflection require compile tests, not runtime counters.
"""
import argparse
from pathlib import Path
import gzip
import json
import re
parser = argparse.ArgumentParser()
parser.add_argument('data', type=Path)
parser.add_argument('--report-only', action='store_true', help='Report coverage without requiring every branch to be reached')
parser.add_argument('--minimum-percent', type=float, default=100.0, help='Minimum branch coverage percentage (default: 100)')
parser.add_argument('--root', type=Path, default=Path.cwd())
parser.add_argument('--output', type=Path, required=True)
args = parser.parse_args()
if not 0 <= args.minimum_percent <= 100:
    parser.error("--minimum-percent must be between 0 and 100")
root = args.root.resolve()
files = {}
for p in args.data.rglob('*.gz'):
    for f in json.load(gzip.open(p))['files']:
        if '/include/' not in f['file'] or 'CLI11' in f['file']:
            continue
        name = 'include/' + f['file'].split('/include/', 1)[1]
        if not (root / name).is_file():
            continue
        source = (root / name).read_text().splitlines()
        valid = set()
        depth = 0
        for n, text in enumerate(source, 1):
            if re.search('\\b(?:if|while|for|switch|catch)\\s*\\(', text) and 'if constexpr' not in text:
                depth = text.count('(') - text.count(')')
                valid.add(n)
            elif depth:
                valid.add(n)
                depth += text.count('(') - text.count(')')
            if '?' in text or '&&' in text or '||' in text:
                valid.add(n)
        state = files.setdefault(name, {})
        for l in f['lines']:
            if l['line_number'] not in valid:
                continue
            bs = l['branches']
            i = 0
            while i < len(bs):
                group = [bs[i]]
                i += 1
                while i < len(bs) and bs[i]['source_block_id'] == group[0]['source_block_id']:
                    group.append(bs[i])
                    i += 1
                if len(group) < 2 or any((b['throw'] for b in group)):
                    continue
                if any((b['throw'] and b['destination_block_id'] == group[0]['source_block_id'] for b in bs)):
                    continue
                line = l['line_number']
                simple = not any((op in source[line - 1] for op in ('&&', '||', '?', 'switch')))
                key = (line, 0 if simple else group[0]['source_block_id'])
                record = state.setdefault(key, {})
                for b in group:
                    dest = int(b['fallthrough']) if simple else b['destination_block_id']
                    record[dest] = record.get(dest, 0) + b['count']
rows = []
excluded = []
for name, decisions in sorted(files.items()):
    source = (root / name).read_text().splitlines()
    program_lines = [i for i, t in enumerate(source, 1) if 'auto program = argc > 0 && argv && argv[0]' in t]
    program_blocks = sorted((block for line, block in decisions if line in program_lines))
    report_catches = [i for i, t in enumerate(source, 1)
                      if '} catch (const std::exception&' in t
                      and any('Cannot write XML report' in next_line for next_line in source[i:i + 8])]
    for (line, block), arcs in sorted(decisions.items()):
        for destination, count in arcs.items():
            reason = None
            if name == 'include/runner.cxx':
                text = source[line - 1].strip()
                if text.startswith(('if (out) suite_log.', 'if (err) suite_log.')) and destination == 0:
                    reason = 'A constructed capture owns both files; failed construction never invokes its destructor.'
                elif line in program_lines and block in program_blocks[:2] and (count == 0):
                    reason = 'Showing help requires a parsed argument, hence argc > 0 and argv is non-null.'
                elif report_catches and line == report_catches[-1] and (destination == 1):
                    reason = 'The report writer and its fixed internal formatters only throw std::exception-derived errors.'
            record = {'file': name, 'line': line, 'block': block, 'edge': destination, 'count': count}
            if reason:
                if count:
                    raise RuntimeError('Excluded outcome became reachable: ' + str(record))
                record['reason'] = reason
                excluded.append(record)
            else:
                rows.append(record)
covered = sum((row['count'] > 0 for row in rows))
result = {'metric': 'reachable runtime source-associated branches', 'covered': covered, 'total': len(rows), 'percent': 100 * covered / len(rows), 'excluded_unreachable_outcomes': excluded, 'branches': rows}
args.output.write_text(json.dumps(result, indent=2) + '\n')
for row in rows:
    if not row['count']:
        print('Uncovered:', row)
print(f"Reachable source coverage: {covered}/{len(rows)} = {result['percent']:.1f}%")
raise SystemExit(not args.report_only and result['percent'] < args.minimum_percent)
