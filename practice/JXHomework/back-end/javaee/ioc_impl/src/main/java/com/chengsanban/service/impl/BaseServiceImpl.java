package com.chengsanban.service.impl;

import com.chengsanban.dao.BaseDao;
import com.chengsanban.service.BaseService;

public class BaseServiceImpl implements BaseService {
    private BaseDao dao;

    public BaseDao getDao() {
        return dao;
    }

    public void setDao(BaseDao dao) {
        this.dao = dao;
    }

    @Override
    public void test() {
        System.out.println("BaseServiceImpl run");
        dao.test();
    }
}
