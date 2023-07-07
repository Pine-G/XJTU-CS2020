package pack;

import java.sql.*;
import java.io.*;

public class InsertThread extends Thread {
    private final Connection conn;
    private final String filename;
    private final String delimiter;
    private final int data_num;
    private final Object lock = new Object();

    public InsertThread(Connection conn, String filename, String delimiter, int data_num) {
        this.conn = conn;
        this.filename = filename;
        this.delimiter = delimiter;
        this.data_num = data_num;
    }

    @Override
    public void run() {
        try {
            // 创建预编译SQL语句
            String sql = "INSERT INTO SC500 (SNO, CNO, GRADE) VALUES (?, ?, ?)";
            PreparedStatement ps = this.conn.prepareStatement(sql);
            // 读取数据文件
            BufferedReader reader = new BufferedReader(new FileReader(this.filename));
            String line = reader.readLine();
            for (int i = 0; i < this.data_num && line != null; i++) {
                synchronized (lock) {
                    // 获取每条记录中各字段值
                    String[] fields = line.split(this.delimiter);
                    // 向SQL语句中填入各字段值
                    ps.setString(1, fields[0]);
                    ps.setString(2, fields[1]);
                    ps.setFloat(3, Float.parseFloat(fields[2]));
                    // 执行插入语句
                    ps.executeUpdate();
                    // 读取下一行记录
                    line = reader.readLine();
                }
            }
            // 关闭数据文件
            reader.close();
            // 关闭预编译SQL语句
            ps.close();
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}
