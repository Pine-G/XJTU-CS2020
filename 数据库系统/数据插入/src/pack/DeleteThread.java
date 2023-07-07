package pack;

import java.sql.*;

public class DeleteThread extends Thread {
    private final Connection conn;
    private final boolean flag;
    private final int data_num;
    private final Object lock = new Object();

    public DeleteThread(Connection conn, boolean flag, int data_num) {
        this.conn = conn;
        this.flag = flag;
        this.data_num = data_num;
    }

    @Override
    public void run() {
        try {
            if (this.flag) {
                Statement stmt = conn.createStatement();
                String sql = "DELETE FROM SC500 WHERE (SNO, CNO) IN " +
                        "(SELECT SNO, CNO FROM SC500 WHERE GRADE < 60 ORDER BY RANDOM() LIMIT 1)";
                for (int i = 0; i < this.data_num; i++) {
                    synchronized (lock) {
                        if (stmt.executeUpdate(sql) == 0){
                            i--;
                        }
                    }
                }
                stmt.close();
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}
