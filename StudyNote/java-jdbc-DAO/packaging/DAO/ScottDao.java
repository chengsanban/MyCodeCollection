package com.packaging.DAO;

import java.util.List;

import com.packaging.entity.Dept;

public interface ScottDao {
    public List<Dept> queryDeptByPar(Dept dept);
}
