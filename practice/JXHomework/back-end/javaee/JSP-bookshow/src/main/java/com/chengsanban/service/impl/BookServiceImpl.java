package com.chengsanban.service.impl;

import com.chengsanban.dao.BookDao;
import com.chengsanban.dao.impl.BookDaoImpl;
import com.chengsanban.entity.Books;
import com.chengsanban.entity.Page;
import com.chengsanban.service.BookService;

import java.sql.SQLException;
import java.util.List;

public class BookServiceImpl implements BookService {
    private BookDao dao = new BookDaoImpl();

    @Override
    public Page getPage(Page page) {
        List<Books> list = dao.queryByPar(null, null);
        page.setPageSize(2);
        page.setPageCount(list.size());
        Integer pageCount = page.getPageCount() == 0 ? 1 :
                (page.getPageCount() % page.getPageSize() == 0 ?
                        (page.getPageCount() / page.getPageSize()):
                        ((page.getPageCount() / page.getPageSize())+1)
                );
        page.setPageCount(pageCount);//如果总记录数为0当前页数1
        page.setPageOffset((page.getCurPage()-1)*page.getPageSize());
        return page;
    }

    @Override
    public int insert(Books book) throws SQLException {
        return dao.insert(book);
    }

    @Override
    public int update(Books book) throws SQLException{
        return dao.update(book);
    }

    @Override
    public int delete(Books book) throws SQLException{
        return dao.delete(book);
    }

    @Override
    public List<Books> queryByPar(Books book, Page page) throws SQLException{
        System.out.println("BookServiceImpl---------");
        return dao.queryByPar(book, page);
    }
}
