package com.chengsanban.dao.impl;

import com.chengsanban.dao.BookDao;
import com.chengsanban.dao.base.BaseDao;
import com.chengsanban.entity.Books;
import com.chengsanban.entity.Page;

import java.sql.Connection;
import java.sql.PreparedStatement;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Date;
import java.util.List;

public class BookDaoImpl extends BaseDao implements BookDao{

    @Override
    public int insert(Books book){
        Connection conn = null;
        try {
            conn = super.getConnection();
            conn.setAutoCommit(false);

            String sql = "INSERT INTO `books` (`name`,`author`) VALUES (?,?)";
            PreparedStatement ps = conn.prepareStatement(sql);
            ps.setObject(1, book.getName());
            ps.setObject(2, book.getAuthor());
            int i = ps.executeUpdate();

            conn.commit();
            return i;
        } catch (SQLException e){
            e.printStackTrace();

            if(conn != null){
                try {
                    conn.rollback();
                } catch (SQLException ex) {
                    ex.printStackTrace();
                }

            }
        } finally {
            super.closeConnection();
        }
        return 0;
    }

    @Override
    public int update(Books book) {
        Connection conn = null;
        try {
            conn = super.getConnection();

            conn.setAutoCommit(false);

            String sql = "UPDATE `books` SET `name` = ? , `author` = ? WHERE `id` = ?";
            PreparedStatement ps = conn.prepareStatement(sql);
            ps.setObject(1, book.getName());
            ps.setObject(2, book.getAuthor());
            ps.setObject(3, book.getId());

            int i = ps.executeUpdate();

            conn.commit();

            return i;

        } catch (SQLException e) {
            e.printStackTrace();
            if(conn!=null){
                try {
                    conn.rollback();
                } catch (SQLException e1) {
                    // TODO Auto-generated catch block
                    e1.printStackTrace();
                }
            }
        } finally {
            super.closeConnection();
        }

        return 0;
    }

    @Override
    public int delete(Books book) {
        Connection conn = null;
        try {
            conn = super.getConnection();

            conn.setAutoCommit(false);

            String sql = "DELETE FROM `books` WHERE `id` = ?";
            PreparedStatement ps = conn.prepareStatement(sql);
            ps.setObject(1, book.getId());

            int i = ps.executeUpdate();

            conn.commit();

            return i;

        } catch (SQLException e) {
            e.printStackTrace();
            if(conn!=null){
                try {
                    conn.rollback();
                } catch (SQLException e1) {
                    // TODO Auto-generated catch block
                    e1.printStackTrace();
                }
            }
        } finally {
            super.closeConnection();
        }

        return 0;
    }


    @Override
    public List<Books> queryByPar(Books book, Page page) {
        List<Books> books = new ArrayList<Books>();

        Connection conn = null;
        try {
            conn = super.getConnection();

            StringBuffer sql = new StringBuffer("SELECT `id`,`name`,`author`,`create_time` "
                    + "FROM `books` WHERE 1=1 ");

            if(book != null){
                System.out.println("add select info");
                if(book.getId() != null){
                    sql.append(" AND ");
                    sql.append(" `id` = ? ");
                }
                if(book.getName() != null){
                    sql.append(" AND ");
                    sql.append(" `name` like ? ");
                }
                if(book.getAuthor() != null){
                    sql.append(" AND ");
                    sql.append(" `anther` like ? ");
                }
            }

            if(page != null){
                if(page.getPageSize()!=null && page.getPageOffset()!=null)
                    sql.append(" LIMIT ?,? ");
            }

            PreparedStatement ps = conn.prepareStatement(sql.toString());

            int index = 0;

            if(book != null){
                if(book.getId() != null){
                    ps.setObject(++index, book.getId());
                }
                if(book.getName() != null){
                    ps.setObject(++index, book.getName());
                }
                if(book.getAuthor() != null){
                    ps.setObject(++index, book.getAuthor());
                }
            }
            if(page != null){
                if(page.getPageSize()!=null && page.getPageOffset()!=null){
                    ps.setObject(++index, page.getPageOffset());
                    ps.setObject(++index, page.getPageSize());
                }
            }

            ResultSet rs = ps.executeQuery();

            SimpleDateFormat sdf = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss");

            while(rs.next()){
                Books b = new Books();

                int id= rs.getInt("id");
                String name = rs.getString("name");
                String author = rs.getString("author");
                String createTime = rs.getString("create_time");

                System.out.println(name);
                System.out.println(author);
                System.out.println(createTime);


                b.setId(id);
                b.setName(name);
                b.setAuthor(author);
                b.setCreateTime(sdf.parse(createTime));

                books.add(b);
            }

        } catch (SQLException e) {
            e.printStackTrace();
        } catch (ParseException e) {
            e.printStackTrace();
        } finally {
            super.closeConnection();
        }

        return books;
    }

}
