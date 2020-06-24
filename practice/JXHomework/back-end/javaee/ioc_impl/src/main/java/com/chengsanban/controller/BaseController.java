package com.chengsanban.controller;

import com.chengsanban.service.BaseService;
import com.chengsanban.web.Controller;

public class BaseController implements Controller {
    private BaseService service;

    public BaseService getService() {
        return service;
    }

    public void setService(BaseService service) {
        this.service = service;
    }

    public void test(){
        System.out.println("BaseController run");
        service.test();
    }
}
