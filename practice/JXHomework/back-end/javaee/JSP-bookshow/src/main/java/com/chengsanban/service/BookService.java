package com.chengsanban.service;

import com.chengsanban.entity.Books;
import com.chengsanban.entity.Page;

import java.sql.SQLException;
import java.util.List;

public interface BookService {
    public Page getPage(Page page);
    public int insert(Books book) throws SQLException;
    public int update(Books book) throws SQLException;
    public int delete(Books book) throws SQLException;
    public List<Books> queryByPar(Books book, Page page) throws SQLException;
}
