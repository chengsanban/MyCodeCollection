package com.chengsanban.web;

import com.chengsanban.mvc.entity.XMLApplicationContext;

import javax.servlet.*;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;
import javax.servlet.http.HttpSession;
import java.io.IOException;
import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;

public class DispatcherServlet implements Servlet {
    @Override
    public void init(ServletConfig servletConfig) throws ServletException {

    }

    @Override
    public ServletConfig getServletConfig() {
        return null;
    }

    @Override
    public void service(ServletRequest req, ServletResponse resp) throws ServletException, IOException {
        HttpServletRequest request = (HttpServletRequest)req;
        HttpServletResponse response = (HttpServletResponse)resp;
        HttpSession session = request.getSession();
        ServletContext application = session.getServletContext();

        String uri = request.getRequestURI();
        String[] uris = uri.split("/");
        String controllerName = uris[2];
        String methodName = uris[3].replaceAll(".do","");

        System.out.println(methodName);
        System.out.println(controllerName);

        XMLApplicationContext xmlac = new XMLApplicationContext();
        Object obj = xmlac.getBean(controllerName);
        try {
            System.out.println(obj.getClass().getName());

            Method method = obj.getClass().getMethod(methodName);
            method.invoke(obj);
        } catch (NoSuchMethodException e) {
            e.printStackTrace();
        } catch (IllegalAccessException e) {
            e.printStackTrace();
        } catch (InvocationTargetException e) {
            e.printStackTrace();
        }

    }

    @Override
    public String getServletInfo() {
        return null;
    }

    @Override
    public void destroy() {

    }
}
