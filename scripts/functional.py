import pickle
import hashlib
import os

def cal_md5(file):
    with open(file, 'r', encoding='utf-8') as f:
        data = f.read()
    return hashlib.md5(data.encode('utf-8')).hexdigest()

test_path = '/home/pi/compiler/functional'
record_path = '/home/pi/compiler/functional.pkl'
sylib_path = '/home/pi/compiler/sylib.a'

if os.path.exists(record_path):
    with open(record_path, 'rb') as f:
        md5_status = pickle.load(f)
else:
    md5_status = {}

pass_num = 0
fail_num = 0

# PASS 0
# Wrong Answer 1
# Execute Error 2
# Execute Timeout 3
# Assemble Error 4


for path, dir_list, file_list in os.walk(test_path):
    for file_name in sorted(file_list):
        if file_name.split('.')[-1] == 'sy':
            case_name = file_name[:-3]
            sy_path = os.path.join(path, file_name)
            asm_path = './functional/' + case_name + '.s'
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
            md5 = cal_md5(asm_path)
            if md5 in md5_status.keys():
                status = md5_status[md5]
                if status == 0:
                    print('\033[1;32mPASS:\033[0m {}'.format(case_name))
                    pass_num += 1
                elif status == 1:
                    print('\033[1;31mFAIL:\033[0m {}\t\033[1;31mWrong Answer\033[0m'.format(case_name))
                    fail_num += 1
                elif status == 2:
                    print('\033[1;31mFAIL:\033[0m {}\t\033[1;31mExecute Error\033[0m'.format(case_name))
                    fail_num += 1
                elif status == 3:
                    print('\033[1;31mFAIL:\033[0m {}\t\033[1;31mExecute Timeout\033[0m'.format(case_name))
                    fail_num += 1
                elif status == 4:
                    print('\033[1;31mFAIL:\033[0m {}\t\033[1;31mAssemble Error\033[0m'.format(case_name))
                    fail_num += 1
                continue
            ret_value = os.system('gcc -o {} {} {} > {} 2>&1'.format(bin_path, asm_path, sylib_path, log_path))
            ret_value >>= 8
            if ret_value != 0:
                print('\033[1;31mFAIL:\033[0m {}\t\033[1;31mAssemble Error\033[0m'.format(case_name))
                fail_num += 1
                md5_status[md5] = 4
                continue
            if os.path.exists(in_path):
                ret_value = os.system('timeout 60s ./{} < {} > {} 2>{}'.format(bin_path, in_path, res_path, log_path))
            else:
                ret_value = os.system('timeout 60s ./{} > {} 2>{}'.format(bin_path, res_path, log_path))
            ret_value >>= 8
            if ret_value == 124:
                print('\033[1;31mFAIL:\033[0m {}\t\033[1;31mExecute Timeout\033[0m'.format(case_name))
                fail_num += 1
                md5_status[md5] = 3
                continue
            elif ret_value == 127:
                print('\033[1;31mFAIL:\033[0m {}\t\033[1;31mExecute Error\033[0m'.format(case_name))
                fail_num += 1
                md5_status[md5] = 2
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
                print('\033[1;31mFAIL:\033[0m {}\t\033[1;31mWrong Answer\033[0m'.format(case_name))
                fail_num += 1
                md5_status[md5] = 1
                continue
            else:
                print('\033[1;32mPASS:\033[0m {}'.format(case_name))
                pass_num += 1
                md5_status[md5] = 0
                continue
with open(record_path, 'wb') as f:
    pickle.dump(md5_status, f)
print('\033[1;33mTotal: {}\t\033[1;32mAccept: {}\t\033[1;31mFail:{}'.format(pass_num + fail_num, pass_num, fail_num))