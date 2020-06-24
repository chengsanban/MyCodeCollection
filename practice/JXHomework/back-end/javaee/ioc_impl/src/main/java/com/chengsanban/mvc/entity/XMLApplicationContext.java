package com.chengsanban.mvc.entity;

import com.chengsanban.mvc.loader.ContextLoader;

public class XMLApplicationContext extends ContextLoader {
    public XMLApplicationContext(){

    }

    public XMLApplicationContext(String path){
        super.init();
    }

    public Object getBean(String name){
        return applicationContext.get(name).getObj();
    }
}
