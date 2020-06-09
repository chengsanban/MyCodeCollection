package com.chengsanban.dao;

import com.chengsanban.entity.Books;
import com.chengsanban.entity.Page;

import java.sql.SQLException;
import java.util.List;

public interface BookDao {
    public int insert(Books book) throws SQLException;
    public int update(Books book);
    public int delete(Books book);
    public List<Books> queryByPar(Books book, Page page);
}
