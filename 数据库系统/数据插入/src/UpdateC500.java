import java.sql.*;
import java.io.*;

public class UpdateC500 {
    public static void main(String[] args) {
        // 数据库用户与密码
        String username = "guo";
        String password = "Bigdata@123";
        // 保存待插入数据的文件名
        String filename = "./data/course.txt";
        // 待插入的数据量
        int data_num = 1000;
        // 字段分隔符
        String delimiter = "; ";
        // 驱动程序
        String driver = "org.postgresql.Driver";
        // 数据库URL
        String url = "jdbc:postgresql://124.70.51.155:26000/my_db";

        try {
            // 加载驱动程序
            Class.forName(driver);
            // 连接数据库
            Connection conn = DriverManager.getConnection(url, username, password);
            // 创建预编译SQL语句
            String sql = "INSERT INTO C500 (CNO, CNAME, PERIOD, CREDIT, TEACHER) VALUES (?, ?, ?, ?, ?)";
            PreparedStatement ps = conn.prepareStatement(sql);
            // 读取数据文件
            BufferedReader reader = new BufferedReader(new FileReader(filename));
            String line = reader.readLine();
            for (int i = 0; i < data_num && line != null; i++) {
                // 获取每条记录中各字段值
                String[] fields = line.split(delimiter);
                // 向SQL语句中填入各字段值
                ps.setString(1, fields[0]);
                ps.setString(2, fields[1]);
                ps.setInt(3, Integer.parseInt(fields[2]));
                ps.setFloat(4, Float.parseFloat(fields[3]));
                ps.setString(5, fields[4]);
                // 执行插入语句
                ps.executeUpdate();
                // 读取下一行记录
                line = reader.readLine();
            }
            // 关闭数据文件
            reader.close();
            // 关闭预编译SQL语句
            ps.close();
            // 关闭数据库连接
            conn.close();
            System.out.println("Update successfully.");
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}