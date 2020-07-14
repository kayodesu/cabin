# -*- coding: utf-8 -*-
# @Author: Yo Ka

import os

passed_cases_count = 0

def collect_one_file(path_name, file_name):
    global passed_cases_count
    result = 'Fail'
    f = open(path_name + "/" + file_name, 'r')
    while True:
        line = f.readline()
        if not line:
            break   
        if line.find(' * Status: Pass') != -1:
            result = 'Pass'
            passed_cases_count += 1
            break   
    
    path_name = path_name.replace('/', '.')
    path_name = path_name.replace('\\', '.')
    i = path_name.find('src.')
    if i == -1: # 根目录
        return '[' + result + '] ' + file_name + '\n'
    else:
        path_name = path_name[i+4:] # jump 'src.'
        return '[' + result + '] ' + path_name + "." + file_name + '\n'

if __name__ == '__main__':
    collection = ''
    cases_count = 0
    for e in os.walk('./test-classes/src'):
        # e[0] # 目录名
        # e[1] # 此目录下的所有子目录
        # e[2] # 此目录下的所有文件
        
        for f in e[2]:
            cases_count += 1
            collection += collect_one_file(e[0], f)            
        collection += '\n'            
            
    # 将结果写回 README.md 中
    
    readme = open('./README.md', 'r')
    buf = readme.read()
    i = buf.find('## Test Cases')
    j = buf.find('## Reference')
    if i == -1 or j == -1 or i >= j:
        print('error'); # todo
    title = "## Test Cases(%d/%d[%.2f%%] Passed)\n" % (passed_cases_count, cases_count, (passed_cases_count/cases_count)*100)   
    buf = buf[:i] + title + collection + buf[j:]
    readme.close()
    
    readme = open('./README.md', 'w')
    readme.write(buf)
    readme.close()

        
    
    
    
    
    