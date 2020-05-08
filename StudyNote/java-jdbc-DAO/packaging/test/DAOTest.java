package com.packaging.test;

import com.packaging.DAO.ScottDao;
import com.packaging.DAO.ScottDao2;
import com.packaging.DAO.imp.ScottDao2Impl;
import com.packaging.DAO.imp.ScottDaoImpl;
import com.packaging.entity.Dept;
import com.packaging.entity.Emp;
import com.packaging.vo.DeptAndEmpVO;

import java.sql.*;
import java.util.EmptyStackException;
import java.util.List;
import java.util.Random;
import java.util.UUID;

public class DAOTest {

    public static void main(String[] args) {
        //testDAO();
        //testDeptAndEmpVO();
        //testDataBaseMetaData();
        //testUUID();
        testGenerateKey();
    }

    //用jdbc的方式解决先创建后查询的问题
    public static void testGenerateKey(){
        Connection conn = null;
        try {
            conn = getConnection();
            conn.setAutoCommit(false);//开事务

            StringBuffer sql = new StringBuffer();
            sql.append("INSERT INTO user (username,password) values (?,MD5(?))");

            PreparedStatement ps = conn.prepareStatement(sql.toString(),Statement.RETURN_GENERATED_KEYS);
            ps.setObject(1, "ybb3");
            ps.setObject(2, "123456");
            int r = ps.executeUpdate();
            if(r == 1){
                ResultSet resultset = ps.getGeneratedKeys();

                if(resultset.next()){
                    int id = resultset.getInt(1);

                    sql = new StringBuffer();
                    sql.append("INSERT INTO user_extends (message,user_id) values (?,?)");

                    ps = conn.prepareStatement(sql.toString());
                    ps.setObject(1, "test message");
                    ps.setObject(2, id);

                    int r2 = ps.executeUpdate();
                    System.out.println(r2);
                }

            }

            conn.commit();
        } catch (SQLException | ClassNotFoundException e) {
            e.printStackTrace();
            if(conn != null)
                try {
                    conn.rollback();
                } catch (SQLException e1) {
                    // TODO Auto-generated catch block
                    e1.printStackTrace();
                }
        } finally {
            if (conn != null)
                try {
                    conn.close();
                } catch (SQLException e) {
                    // TODO Auto-generated catch block
                    e.printStackTrace();
                }
        }
    }

    //测试当事务开启，新建完用户，就要查询的情况，因为姓名等不能作为唯一查询条件，自动id我们不能获取
    //需要加一个uuid来确定唯一
    public static void testUUID(){
        Connection conn = null;
        try {
            conn = getConnection();

            String uuid = UUID.randomUUID().toString();
            Random random = new Random();
            uuid+=random;

            conn.setAutoCommit(false);//开事务

            StringBuffer sql = new StringBuffer();
            sql.append("INSERT INTO user (username,password,uuid) values (?,MD5(?),?)");

            PreparedStatement ps = conn.prepareStatement(sql.toString());

            ps.setObject(1, "ybb2");
            ps.setObject(2, "123456");
            ps.setObject(3, uuid);

            int r = ps.executeUpdate();

            if(r == 1){
                sql = new StringBuffer();
                sql.append("select id from user where uuid = ?");
                ps = conn.prepareStatement(sql.toString());
                ps.setObject(1, uuid);

                ResultSet rs = ps.executeQuery();
                if(rs.next()){
                    int id = rs.getInt(1);

                    sql = new StringBuffer();
                    sql.append("INSERT INTO user_extends (message,user_id) values (?,?)");

                    ps = conn.prepareStatement(sql.toString());
                    ps.setObject(1, "test message");
                    ps.setObject(2, id);

                    int r2 = ps.executeUpdate();
                    System.out.println(r2);
                }

            }

            conn.commit();
        } catch (SQLException | ClassNotFoundException e) {
            e.printStackTrace();
            if(conn != null)
                try {
                    conn.rollback();
                } catch (SQLException e1) {
                    // TODO Auto-generated catch block
                    e1.printStackTrace();
                }
        } finally {
            if (conn != null)
                try {
                    conn.close();
                } catch (SQLException e) {
                    // TODO Auto-generated catch block
                    e.printStackTrace();
                }
        }
    }
    /*
    * 测试Dao2的情况
    * */
    private static void testDAO(){
        ScottDao2 dao = new ScottDao2Impl();
        //List<Dept> r= dao.query(new Dept().setDeptno(40));
        Emp e = new Emp();
        e.setDeptno(10);
        List<Emp> r= dao.query(e);
        System.out.println(r);
    }

    /*
     * 测试VO
     * */
    private static void testDeptAndEmpVO(){
        Connection conn = null;
        try {
            conn = getConnection();

            StringBuffer sql = new StringBuffer("select e.empno , e.ename , d.dname , d.deptno "
                    + "from emp e, dept d where e.deptno = d.deptno");

            PreparedStatement ps = conn.prepareStatement(sql.toString());

            ResultSet rs = ps.executeQuery();
            //处理好的数据，通常可以称之为vo
            while(rs.next()){
                DeptAndEmpVO vo = new DeptAndEmpVO();
                vo.setEmpno(rs.getInt("empno"));
                vo.setEname(rs.getString("ename"));
                vo.setDeptno(rs.getInt("deptno"));
                vo.setDname(rs.getString("dname"));
                System.out.println(vo);
            }

        } catch (SQLException | ClassNotFoundException e) {
            e.printStackTrace();
        } finally {
            if (conn != null)
                try {
                    conn.close();
                } catch (SQLException e) {
                    // TODO Auto-generated catch block
                    e.printStackTrace();
                }
        }
    }

    /*
     * 测试databasemetadata和reslutSetmetadata
     * */
    private static void testDataBaseMetaData(){
        Connection conn = null;
        try {
            //获取数据库元数据
            conn = getConnection();
            DatabaseMetaData md = conn.getMetaData();
            System.out.println(md.getURL());
            System.out.println(md.getUserName());
            System.out.println(md.isReadOnly());
            System.out.println(md.getDatabaseProductName());
            System.out.println(md.getDatabaseProductVersion());
            System.out.println(md.getDriverName());
            System.out.println(md.getDriverVersion());

            //获取结果元数据
            PreparedStatement ps = conn.prepareStatement("select * from emp");
            ResultSet resultSet = ps.executeQuery();

            if(resultSet.next()){
                ResultSetMetaData rsmd = resultSet.getMetaData();
                int cc = rsmd.getColumnCount();
                for(int i = 0 ; i < cc ; i ++){
                    System.out.println(rsmd.getColumnName(i+1));
                    System.out.println(rsmd.getColumnTypeName(i+1));
                    System.out.println(rsmd.getColumnDisplaySize(i+1));
                    System.out.println(rsmd.isNullable(i+1));
                    System.out.println(rsmd.isAutoIncrement(i+1));

                }
            }

        } catch (SQLException | ClassNotFoundException e) {
            e.printStackTrace();
        } finally {
            if (conn != null)
                try {
                    conn.close();
                } catch (SQLException e) {
                    // TODO Auto-generated catch block
                    e.printStackTrace();
                }
        }
    }

    /*
     * 获取数据库链接
     * */
    private static Connection getConnection() throws SQLException, ClassNotFoundException {
        final String driver = "com.mysql.cj.jdbc.Driver";
        final String url = "jdbc:mysql://127.0.0.1:3306/scoot_test";
        final String user = "root";
        final String pw = "mysql123456";
        Class.forName(driver);
        Connection conn = DriverManager.getConnection(url, user, pw);//这个异常往外抛
        return conn;
    }

}
