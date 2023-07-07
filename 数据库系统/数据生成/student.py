from faker import Faker

data_size = 5000

sno_list = []
name_list = []
sex_list = []
birth_list = []
height_list = []
dorm_list = []

sno_exist = ['01032010', '01032023', '01032001', '01032005', '01032112',
             '03031011', '03031014', '03031051', '03031009', '03031033',
             '03031056']
dorm_exist = ['东14舍221', '东14舍221', '东1舍312', '东1舍312', '东14舍221',
              '东2舍104', '东18舍421', '东18舍422', '东2舍104', '东18舍423',
              '东2舍305']

# 女生住东1-东5 男生住东14-18
# 假设每栋楼8层 每层25间宿舍 每间宿舍4人
dorm_male = {}
building_male = ['东14舍', '东15舍', '东16舍', '东17舍', '东18舍']
for building in building_male:
    for i in range(1, 9):
        for j in range(1, 26):
            dorm_male[f'{building}{i*100+j}'] = 4

dorm_female = {}
building_female = ['东1舍', '东2舍', '东3舍', '东4舍', '东5舍']
for building in building_female:
    for i in range(1, 9):
        for j in range(1, 26):
            dorm_female[f'{building}{i*100+j}'] = 4

for dorm in dorm_exist:
    if dorm in dorm_male:
        dorm_male[dorm] -= 1
    else:
        dorm_female[dorm] -= 1

# 随机生成学生信息
fake = Faker('zh_CN')
for i in range(data_size):
    # 生成学号
    sno = f'0{fake.random.randrange(1032000, 3032000, 1)}'
    while sno in sno_exist:
        sno = f'0{fake.random.randrange(1032000, 3032000, 1)}'
    sno_exist.append(sno)
    # 按照男女比例7：3生成数据
    if i % 10 <= 6:
        name = fake.name_male()
        sex = '男'
        height = fake.random.randrange(160, 190) / 100
        # 分配宿舍
        dorm = fake.random.sample(dorm_male.keys(), 1)[0]
        while dorm_male[dorm] == 0:
            dorm = fake.random.sample(dorm_male.keys(), 1)[0]
        dorm_male[dorm] -= 1
    else:
        name = fake.name_female()
        sex = '女'
        height = fake.random.randrange(150, 180) / 100
        # 分配宿舍
        dorm = fake.random.sample(dorm_female.keys(), 1)[0]
        while dorm_female[dorm] == 0:
            dorm = fake.random.sample(dorm_female.keys(), 1)[0]
        dorm_female[dorm] -= 1
    # 随机生日
    birth = fake.date_of_birth(minimum_age=19, maximum_age=23).strftime('%Y-%m-%d')

    sno_list.append(sno)
    name_list.append(name)
    sex_list.append(sex)
    birth_list.append(birth)
    height_list.append(height)
    dorm_list.append(dorm)

# 保存数据
with open('student.txt', mode='w', encoding='utf-8') as f:
    for i in range(data_size):
        f.write(sno_list[i] + ' ')
        f.write(name_list[i] + ' ')
        f.write(sex_list[i] + ' ')
        f.write(birth_list[i] + ' ')
        f.write(str(height_list[i]) + ' ')
        f.write(dorm_list[i] + '\n')
