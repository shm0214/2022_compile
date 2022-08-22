import pickle
import hashlib
import os
import re
import csv
import time

def cal_md5(file):
    with open(file, 'r', encoding='utf-8') as f:
        data = f.read()
    return hashlib.md5(data.encode('utf-8')).hexdigest()

test_path = '/home/pi/compiler/performance'
record_path = '/home/pi/compiler/performance.pkl'
in_md5_path = '/home/pi/compiler/performance_in.pkl'
sylib_path = '/home/pi/compiler/sylib.a'

csv_path = '/home/pi/compiler/data.csv'

if os.path.exists(record_path):
    with open(record_path, 'rb') as f:
        md5_time = pickle.load(f)
else:
    md5_time = {}

if os.path.exists(in_md5_path):
    with open(in_md5_path, 'rb') as f:
        name_md5 = pickle.load(f)
else:
    name_md5 = {}

pass_num = 0
fail_num = 0

# PASS 0
# Wrong Answer 1
# Execute Error 2
# Execute Timeout 3
# Assemble Error 4

name_list = ['time']
result_list = [time.strftime("%Y-%m-%d %H:%M:%S", time.localtime())]

for path, dir_list, file_list in os.walk(test_path):
    for file_name in sorted(file_list):
        if file_name.split('.')[-1] == 'sy':
            case_name = file_name[:-3]
            name_list.append(case_name)
            sy_path = os.path.join(path, file_name)
            asm_path = './performance/' + case_name + '.s'
            log_path = case_name + '.log'
            bin_path = case_name + '.bin'
            res_path = case_name + '.res'
            in_path = sy_path.replace('.sy', '.in')
            out_path = sy_path.replace('.sy', '.out')
            # ret_value = os.system('timeout 10s build/compiler -S {} -o {} -O2 > {} 2>&1'.format(sy_path, asm_path, log_path))
            # ret_value >>= 8
            # if ret_value == 124:
            #     print('\033[1;31mFAIL:\033[0m {}\t\033[1;31mCompile Timeout\033[0m'.format(case_name))
            #     fail_num += 1
            #     continue
            # elif ret_value != 0:
            #     print('\033[1;31mFAIL:\033[0m {}\t\033[1;31mCompile Error\033[0m'.format(case_name))
            #     fail_num += 1
            #     continue
            asm_md5 = cal_md5(asm_path)
            if case_name in name_md5.keys():
                in_md5 = name_md5[case_name]
            else:
                in_md5 = cal_md5(in_path)
                name_md5[case_name] = in_md5
            md5 = asm_md5 + in_md5
            if md5 in md5_time.keys():
                time = md5_time[md5]
                result_list.append(time)
                print('\033[1;32mPASS:\033[0m {}\t {}us'.format(case_name, time))
                pass_num += 1
                continue
            ret_value = os.system('gcc -o {} {} {} > {} 2>&1'.format(bin_path, asm_path, sylib_path, log_path))
            ret_value >>= 8
            if ret_value != 0:
                result_list.append(-1)
                print('\033[1;31mFAIL:\033[0m {}\t\033[1;31mAssemble Error\033[0m'.format(case_name))
                fail_num += 1
                continue
            ret_value = os.system('timeout 60s ./{} < {} > {} 2>{}'.format(bin_path, in_path, res_path, log_path))
            ret_value >>= 8
            if ret_value == 124:
                result_list.append(-1)
                print('\033[1;31mFAIL:\033[0m {}\t\033[1;31mExecute Timeout\033[0m'.format(case_name))
                fail_num += 1
                continue
            elif ret_value == 127:
                result_list.append(-1)
                print('\033[1;31mFAIL:\033[0m {}\t\033[1;31mExecute Error\033[0m'.format(case_name))
                fail_num += 1
                continue
            with open(res_path, 'a+') as f:
                f.seek(0)
                lines = f.readlines()
                if len(lines):
                    line = lines[-1]
                    if len(line):
                        char = line[-1]
                        if char != '\n':
                            f.write('\n')
                f.write('{}\n'.format(ret_value))
            ret_value = os.system('diff -b {} {} >/dev/null 2>&1'.format(res_path, out_path))
            ret_value >>= 8
            if ret_value:
                result_list.append(-1)
                print('\033[1;31mFAIL:\033[0m {}\t\033[1;31mWrong Answer\033[0m'.format(case_name))
                fail_num += 1
                continue
            else:
                with open(log_path, 'r') as f:
                    lines = f.readlines()
                for line in lines[::-1]:
                    if line[:5] == 'TOTAL':
                        res = re.search(r'([0-9]+)H-([0-9]+)M-([0-9]+)S-([0-9]+)us', line)
                        h = int(res.group(1))
                        m = int(res.group(2))
                        s = int(res.group(3))
                        us = int(res.group(4))
                        total_s = 3600 * h + 60 * m + s
                        total_us = total_s * 1000000 + us
                        md5_time[md5] = total_us
                result_list.append(total_us)
                print('\033[1;32mPASS:\033[0m {}\t {}us'.format(case_name, total_us))
                pass_num += 1



with open(record_path, 'wb') as f:
    pickle.dump(md5_time, f)
with open(in_md5_path, 'wb') as f:
    pickle.dump(name_md5, f)
with open(csv_path, 'a') as f:
    writer = csv.writer(f)
    # writer.writerow(name_list)
    writer.writerow(result_list)

print('\033[1;33mTotal: {}\t\033[1;32mAccept: {}\t\033[1;31mFail:{}'.format(pass_num + fail_num, pass_num, fail_num))