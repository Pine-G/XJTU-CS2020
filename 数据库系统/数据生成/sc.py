import random

data_size = 200000
limit = 100     # 每名学生选课上限100门
student_limit = {}
primary_key = {}
sno_list = []
cno_list = []
grade_list = []

with open('student.txt', mode='r', encoding='utf-8') as f:
    student = f.readlines()

with open('course.txt', mode='r', encoding='utf-8') as f:
    course = f.readlines()

student_num = len(student)
course_num = len(course)
i = 0
while i < data_size:
    sno = student[random.randrange(student_num)].split(' ')[0]
    if sno not in student_limit:
        student_limit[sno] = 0
    else:
        if student_limit[sno] < limit:
            student_limit[sno] += 1
        else:
            continue

    cno = course[random.randrange(course_num)].split('; ')[0]

    if (sno, cno) in primary_key:
        continue

    primary_key[(sno, cno)] = True
    sno_list.append(sno)
    cno_list.append(cno)

    grade = round(random.uniform(0, 100) / 0.5) * 0.5
    grade_list.append(grade)

    i += 1

with open('sc.txt', mode='w', encoding='utf-8') as f:
    for i in range(data_size):
        f.write(sno_list[i] + ' ')
        f.write(cno_list[i] + ' ')
        f.write(str(grade_list[i]) + '\n')
