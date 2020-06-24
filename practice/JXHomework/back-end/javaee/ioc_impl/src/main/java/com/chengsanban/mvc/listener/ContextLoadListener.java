package com.chengsanban.mvc.listener;

import com.chengsanban.mvc.loader.ContextLoader;

import javax.servlet.ServletContextEvent;
import javax.servlet.ServletContextListener;

public class ContextLoadListener extends ContextLoader implements ServletContextListener{
    @Override
    public void contextInitialized(ServletContextEvent event) {
        String realPath = event.getServletContext().getRealPath(""); //获取真实路径
        System.out.println("路径是：" + realPath);
        super.init(realPath);
    }

    @Override
    public void contextDestroyed(ServletContextEvent event) {

    }
}
