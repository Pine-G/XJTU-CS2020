from selenium import webdriver
from selenium.webdriver.common.by import By
import time

# 学号与密码
NetID = None
password = None

# 创建WebDriver对象
driver = webdriver.Edge()

# 打开本科教务平台
url = 'http://ehall.xjtu.edu.cn/new/index.html?browser=no'
driver.get(url)

# 点击登录
login_button = driver.find_element(By.ID, 'ampHasNoLogin')
login_button.click()

# 输入学号和密码
user_box = driver.find_element(By.NAME, 'username')
password_box = driver.find_element(By.NAME, 'pwd')
user_box.send_keys(NetID)
password_box.send_keys(password)

# 统一身份认证网关
login_button = driver.find_element(By.CSS_SELECTOR, '.loginState > #account_login')
driver.execute_script('arguments[0].click();', login_button)

# 全校课表查询
time.sleep(3)
app_list = driver.find_element(By.CSS_SELECTOR, '.amp-aside-box-mini-item > .icon-liebiao1')
driver.execute_script('arguments[0].click();', app_list)
course_table = driver.find_element(By.CSS_SELECTOR, '.appFlag:nth-child(10) > .amp-str-cut')
driver.execute_script('arguments[0].click();', course_table)

# 切换页面
new_window = driver.window_handles[-1]
driver.switch_to.window(new_window)

# 获取课程信息
with open('course_crawler.txt', mode='w') as f:
    for i in range(400):
        time.sleep(3)
        for j in range(10):
            cno1 = f'id("row{j}qxkcb-index-table")/TD[3]/SPAN[1]'
            cno2 = f'id("row{j}qxkcb-index-table")/TD[5]/SPAN[1]'
            cname = f'id("row{j}qxkcb-index-table")/TD[4]/SPAN[1]'
            college = f'id("row{j}qxkcb-index-table")/TD[6]/SPAN[1]'
            period = f'id("row{j}qxkcb-index-table")/TD[8]/SPAN[1]'
            credit = f'id("row{j}qxkcb-index-table")/TD[9]/SPAN[1]'
            teacher = f'id("row{j}qxkcb-index-table")/TD[10]/SPAN[1]'
            f.write(driver.find_element(By.XPATH, cno1).get_attribute('title') + '; ')
            f.write(driver.find_element(By.XPATH, cno2).get_attribute('title') + '; ')
            f.write(driver.find_element(By.XPATH, cname).get_attribute('title') + '; ')
            f.write(driver.find_element(By.XPATH, college).get_attribute('title') + '; ')
            f.write(driver.find_element(By.XPATH, period).get_attribute('title') + '; ')
            f.write(driver.find_element(By.XPATH, credit).get_attribute('title') + '; ')
            f.write(driver.find_element(By.XPATH, teacher).get_attribute('title') + '\n')
        next_page = driver.find_element(By.CSS_SELECTOR, '#pagerqxkcb-index-table .icon-keyboardarrowright')
        driver.execute_script('arguments[0].click();', next_page)

# 关闭浏览器
driver.quit()
