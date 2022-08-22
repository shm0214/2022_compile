import os
import hashlib

def get_file(file):
    with open(file, 'r', encoding='utf-8') as f:
        data = f.read()
    return data

def cal_md5(file):
    data = get_file(file)
    return hashlib.md5(data.encode('utf-8')).hexdigest()



functional_path = '/home/ubuntu/compile/functional'
performance_path = '/home/ubuntu/compile/performance'

function_asm_path = './functional/'
performance_asm_path = './performance/'

if not os.path.exists(function_asm_path):
    os.makedirs(function_asm_path)
if not os.path.exists(performance_asm_path):
    os.makedirs(performance_asm_path)

pass_num = 0
fail_num = 0

for path, dir_list, file_list in os.walk(functional_path):
    for file_name in sorted(file_list):
        case_name = file_name[:-3]
        sy_path = os.path.join(path, file_name)
        asm_path = function_asm_path + case_name + '.s'
        log_path = case_name + '.log'
        ret_value = os.system('timeout 60s build/compiler -S {} -o {} > {} 2>&1'.format(sy_path, asm_path, log_path))
        ret_value >>= 8
        if ret_value == 124:
            print('\033[1;31mFAIL:\033[0m {}\t\033[1;31mCompile Timeout\033[0m'.format(case_name))
            fail_num += 1
            continue
        elif ret_value != 0:
            print('\033[1;31mFAIL:\033[0m {}\t\033[1;31mCompile Error\033[0m'.format(case_name))
            fail_num += 1
            continue
        else:
            pass_num += 1
            print('\033[1;32mCompile Success:\033[0m {}'.format(case_name))

cache = {}

for path, dir_list, file_list in os.walk(performance_path):
    for file_name in sorted(file_list):
        case_name = file_name[:-3]
        sy_path = os.path.join(path, file_name)
        asm_path = performance_asm_path + case_name + '.s'
        log_path = case_name + '.log'
        md5 = cal_md5(sy_path)
        if md5 in cache.keys():
            data = cache[md5]
            if isinstance(data, str):
                with open(asm_path, 'w') as f:
                    f.write(data)
                ret_value = 0
            elif isinstance(data, int):
                ret_value = data
                with open(asm_path, 'w') as f:
                    pass
        else:
            ret_value = os.system('timeout 60s build/compiler -S {} -o {} -O2 > {} 2>&1'.format(sy_path, asm_path, log_path))
            ret_value >>= 8
            if ret_value == 0:
                data = get_file(asm_path)
                cache[md5] = data
            else:
                cache[md5] = ret_value
        if ret_value == 124:
            print('\033[1;31mFAIL:\033[0m {}\t\033[1;31mCompile Timeout\033[0m'.format(case_name))
            fail_num += 1
            continue
        elif ret_value != 0:
            print('\033[1;31mFAIL:\033[0m {}\t\033[1;31mCompile Error\033[0m'.format(case_name))
            fail_num += 1
            continue
        else:
            pass_num += 1
            print('\033[1;32mCompile Success:\033[0m {}'.format(case_name))

print('\033[1;33mTotal: {}\t\033[1;32mSuccess: {}\t\033[1;31mFail:{}'.format(pass_num + fail_num, pass_num, fail_num))