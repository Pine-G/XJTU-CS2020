import java.sql.*;
import pack.InsertThread;
import pack.DeleteThread;

public class UpdateSC500 {
    public static void main(String[] args) {
        // 数据库用户与密码
        String username = "guo";
        String password = "Bigdata@123";
        // 保存待插入数据的文件名
        String filename = "./data/sc.txt";
        // 待插入的数据量
        int insert_data_num = 200000;
        // 待删除的数据量
        int delete_data_num = 200;
        // 字段分隔符
        String delimiter = " ";
        // 是否删除数据
        boolean flag = false;
        // 驱动程序
        String driver = "org.postgresql.Driver";
        // 数据库URL
        String url = "jdbc:postgresql://124.70.51.155:26000/my_db";

        try {
            // 加载驱动程序
            Class.forName(driver);
            // 连接数据库
            Connection conn = DriverManager.getConnection(url, username, password);
            // 创建两个线程
            InsertThread thread1 = new InsertThread(conn, filename, delimiter, insert_data_num);
            DeleteThread thread2 = new DeleteThread(conn, flag, delete_data_num);
            thread1.start();
            thread2.start();
            thread1.join();
            thread2.join();
            // 关闭数据库连接
            conn.close();
            System.out.println("Update successfully.");
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}