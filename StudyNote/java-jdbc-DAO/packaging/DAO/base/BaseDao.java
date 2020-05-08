package com.packaging.DAO.base;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.SQLException;
import java.util.Properties;

public abstract class BaseDao {

    private String driver;
    private String url;
    private String user;
    private String pw;

    public BaseDao(){
        //JDK提供的，专门用于读取Properties文件的API
        Properties p = new Properties();
        String path = BaseDao.class.getClassLoader().getResource("jdbc.properties").getPath();
        try {
            //加载配置文件
            p.load(new FileInputStream(new File(path)));
            driver = p.getProperty("DRIVER");
            url = p.getProperty("URL");
            user = p.getProperty("USER");
            pw = p.getProperty("PASSWORD");
            //加载驱动
            Class.forName(driver);
        } catch (FileNotFoundException e) {
            e.printStackTrace();
        } catch (IOException e) {
            e.printStackTrace();
        } catch (ClassNotFoundException e) {
            e.printStackTrace();
        }
    }

    private Connection conn = null;

    protected Connection getConnection() throws SQLException{
        conn = DriverManager.getConnection(url, user, pw);//这个异常往外抛
        return conn;
    }

    protected void closeConnection(){
        if(conn != null){
            try {
                this.conn.close();
            } catch (SQLException e) {
                e.printStackTrace();
            }
        }
    }

}
