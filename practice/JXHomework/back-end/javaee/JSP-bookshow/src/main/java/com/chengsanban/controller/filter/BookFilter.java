package com.chengsanban.controller.filter;

import com.chengsanban.controller.BookController;

import javax.servlet.*;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;
import javax.servlet.http.HttpSession;
import java.io.IOException;
import java.util.Arrays;

public class BookFilter implements Filter{
    @Override
    public void init(FilterConfig filterConfig) throws ServletException {

    }

    @Override
    public void doFilter(ServletRequest req, ServletResponse resp, FilterChain chain) throws IOException, ServletException {
        HttpServletRequest hreq = (HttpServletRequest) req;
        HttpServletResponse hresp = (HttpServletResponse) resp;
        HttpSession session = hreq.getSession();

        String uri = hreq.getRequestURI();
        //System.out.println(Arrays.toString(uri.split("/")));
        String nameSpace = uri.split("/")[1];
        String method = uri.split("/")[2];

//        System.out.println(nameSpace);
//        System.out.println(method);

        try {
            if("webtest".equals(nameSpace)){

                BookController bookController = new BookController();
                bookController.setRequest(hreq);
                bookController.setResponse(hresp);
                bookController.setSession(session);

                hreq.setCharacterEncoding("UTF-8");

                if("toList.do".equalsIgnoreCase(method)){
                    System.out.println("-------3");
                    bookController.toList();
                }else if("del.do".equals(method)){
                    bookController.del();
                }else if("toUpdate.do".equals(method)){
                    bookController.toUpdate();
                }else if("insertOrUpdate.do".equals(method)){
                    bookController.insertOrUpdate();
                }
            }
        } catch (Exception e){
            e.printStackTrace();
        }

    }

    @Override
    public void destroy() {

    }
}
