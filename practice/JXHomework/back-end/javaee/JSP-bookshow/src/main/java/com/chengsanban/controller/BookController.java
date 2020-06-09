package com.chengsanban.controller;

import com.chengsanban.controller.base.BaseController;
import com.chengsanban.entity.Books;
import com.chengsanban.entity.Page;
import com.chengsanban.service.BookService;
import com.chengsanban.service.impl.BookServiceImpl;

import java.util.List;

public class BookController extends BaseController {
    private BookService service = new BookServiceImpl();

    //展示列表
    public void toList() throws Exception {
        Books book = new Books();
        Page page = new Page();

        //计算page条件
        String curPageStr = super.getRequest().getParameter("curPage");
        if(curPageStr == null) curPageStr = "1";
        page.setCurPage(Integer.valueOf(curPageStr));
        page = service.getPage(page);

        List<Books> bookList = service.queryByPar(book, page);

        super.getRequest().setAttribute("booklist", bookList);
        super.getRequest().setAttribute("page", page);

        super.getRequest().getRequestDispatcher("booklist.jsp").forward(getRequest(), getResponse());
    }

    //删除
    public void del() throws Exception {
        String id = super.getRequest().getParameter("id");
        Books book = new Books();
        if(id!=null){
            book.setId(Integer.valueOf(id));
            int i = service.delete(book);
            if(i == 1){
                super.getRequest().setAttribute("result", "success");
            }else{
                super.getRequest().setAttribute("result", "error");
            }
        }else{
            super.getRequest().setAttribute("result", "error");
        }
        //重定向到列表页
        super.getResponse().sendRedirect("toList.do");
    }

    //修改页面
    public void toUpdate() throws Exception {
        String id = super.getRequest().getParameter("id");
        Books book = new Books();
        if(id!=null){
            book.setId(Integer.valueOf(id));
            List<Books> booksList = service.queryByPar(book, null);
            if(booksList.size() == 1){
                book = booksList.get(0);
                super.getRequest().setAttribute("book", book);
                super.getRequest().setAttribute("result", "success");
                super.getRequest().getRequestDispatcher("insertOrUpdateBook.jsp").forward(getRequest(),getResponse());
                return; //防止重复转发
            }else{
                super.getRequest().setAttribute("result", "error");
            }
        }else{
            super.getRequest().setAttribute("result", "error");
        }
        //重定向到列表页
        super.getResponse().sendRedirect("toList.do");
    }
    //新增或者更新
    public void insertOrUpdate() throws Exception {
        //如果有ID提交
        String id = super.getRequest().getParameter("id");
        Books book = new Books();
        int i = 0;

        String name = super.getRequest().getParameter("name");
        String author = super.getRequest().getParameter("author");
        book.setName(name);
        book.setAuthor(author);

        System.out.println("新增或删除数据--------");
        System.out.println(id);
        System.out.println(name);
        System.out.println(author);
        System.out.println("-------------------");

        if(id!=null && !"".equals(id)){//修改
            book.setId(Integer.valueOf(id));
            i = service.update(book);
        }else{//新增
            i = service.insert(book);
        }
        //重定向到列表页
        super.getResponse().sendRedirect("toList.do");
    }
}
