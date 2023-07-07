import random

data_size = 1000
cno_exist = ['CS-01', 'CS-02', 'CS-04', 'CS-05', 'EE-01', 'EE-02', 'EE-03']
cno_list = []
name_list = []
period_list = []
credit_list = []
teacher_list = []

with open('course_crawler.txt', mode='r') as f:
    lines = f.readlines()

total_size = len(lines)
i = 0
while i < data_size:
    line = lines[random.randrange(total_size)][:-1]
    record = line.split('; ')

    name = record[2]
    teacher = record[6].split(',')[0]
    if len(name) > 16 or len(teacher) > 3:
        continue

    cno = record[0]
    if cno[:4] == 'COMP':
        cno = 'CS'
    else:
        cno = cno[:2]
    num = abs(hash(record[0][4:10])) % 100
    cno += f'-{num}' if num > 9 else f'-0{num}'
    if cno in cno_exist or cno in cno_list:
        continue

    period = record[4]
    credit = record[5]

    cno_list.append(cno)
    name_list.append(name)
    period_list.append(period)
    credit_list.append(credit)
    teacher_list.append(teacher)
    i += 1

with open('course.txt', mode='w', encoding='utf-8') as f:
    for i in range(data_size):
        f.write(cno_list[i] + '; ')
        f.write(name_list[i] + '; ')
        f.write(period_list[i] + '; ')
        f.write(credit_list[i] + '; ')
        f.write(teacher_list[i] + '\n')
