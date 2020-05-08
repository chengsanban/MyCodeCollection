package com.packaging.DAO;

import java.util.List;

public interface ScottDao2 {
    public <T> List<T> query(T t);
    public <T> T queryById(Integer id,Class<?> cla);
    public <T> int insert(T t);
    public <T> int insertList(List<T> ts);
    public <T> int update(T t);
    public <T> int deleteById(Integer id,Class<?> cla);
}
