package com.packaging.DAO.imp;

import com.packaging.DAO.ScottDao2;
import com.packaging.DAO.base.BaseDao;
import com.packaging.annt.Column;
import com.packaging.annt.Table;

import java.lang.reflect.Field;
import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
import java.math.BigDecimal;
import java.sql.Connection;
import java.sql.PreparedStatement;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.util.*;

public class ScottDao2Impl extends BaseDao implements ScottDao2{

    public <T> List<T> query(T t) {
        /*
         * 1.要知道我要查询的是哪张表
         * 2.要知道我要查询那些列
         * 3.如何映射（列-属性/表-对象）
         * 4.最终的查询结果，根据传入的数据，动态的查询
         */
        List<T> resultList = new ArrayList<T>();
        try {
            Connection conn = super.getConnection();
            //反射
            Class<?> clazz = t.getClass();
            //获取表名
            String tableName = "";
            Table table = clazz.getAnnotation(Table.class);
            //如果加了注解，表名已注解当中的为准
            if(table != null){
                tableName = table.value();
                if("".equals(tableName)){
                    tableName = clazz.getSimpleName();
                }
            }else{
                //如果没加注解，表名就是类名
                tableName = clazz.getSimpleName();
            }

            //列名
            Set<String> columnNames = new HashSet<String>();
            //where查询条件
            Map<String,String> wheres = new HashMap<String,String>();
            //获取基本查询条件，解析实体类的注解
            for(Field f : clazz.getDeclaredFields()){
                Boolean isSelect = true;
                String condition = "=";
                String columnName = "";
                Column ca = f.getAnnotation(Column.class);
                if(ca == null){
                    columnName = f.getName();
                }else{
                    isSelect = ca.select();
                    condition = ca.condition();
                    columnName = ca.value();
                    if("".equals(columnName)){
                        columnName = f.getName();
                    }
                }
                if(isSelect){//允许作为查询条件，才加入
                    columnNames.add(columnName);
                }
                //获取属性值，有值再PUT
                //获取该属性的get方法
                String getMethodName = "get"
                        +String.valueOf(f.getName().charAt(0)).toUpperCase()
                        +f.getName().substring(1);

                Method getMethod = clazz.getMethod(getMethodName);
                Object value = getMethod.invoke(t);
                if(value != null){
                    wheres.put(columnName, condition);
                }
            }
            //根据实际情况，组装SQL
            StringBuilder sql = new StringBuilder("SELECT ");

            for(String column : columnNames){
                sql.append(column);
                sql.append(",");
            }
            String sqlStr = sql.toString().substring(0, sql.toString().length()-1);
            sql = new StringBuilder(sqlStr);
            sql.append(" FROM ");
            sql.append(tableName);
            sql.append(" WHERE 1=1 ");
            for(String key : wheres.keySet()){
                sql.append(" AND ");
                sql.append(key);
                sql.append(wheres.get(key));
                sql.append("?");
            }

            System.out.println(sql.toString());

            //获取解析型查询通道
            PreparedStatement ps = conn.prepareStatement(sql.toString());
            //放条件
            int index = 0;
            for(Field f : clazz.getDeclaredFields()){
                //填值
                String getMethodName = "get"
                        +String.valueOf(f.getName().charAt(0)).toUpperCase()
                        +f.getName().substring(1);

                Method getMethod = clazz.getMethod(getMethodName);
                Object value = getMethod.invoke(t);
                if(value != null){
                    ps.setObject(++index, value);
                }
            }
            //查询结果
            ResultSet result = ps.executeQuery();
            while(result.next()){
                //查询结果要放的地方
                Object item = t.getClass().newInstance();

                for(Field f : clazz.getDeclaredFields()){
                    Boolean isSelect = true;
                    String columnName = "";
                    Column ca = f.getAnnotation(Column.class);
                    if(ca == null){
                        columnName = f.getName();
                    }else{
                        isSelect = ca.select();
                        columnName = ca.value();
                        if("".equals(columnName)){
                            columnName = f.getName();
                        }
                    }
                    if(isSelect){//允许作为查询条件，才获取该列的值
                        Object obj = result.getObject(columnName);
                        if(obj != null){
                            String setMethodName = "set"
                                    +String.valueOf(f.getName().charAt(0)).toUpperCase()
                                    +f.getName().substring(1);
                            //获取set方法
                            Method setMethod = clazz.getMethod(setMethodName, f.getType());
                            if(Long.class.getName().equals(obj.getClass().getName())){
                                //整形
                                obj = f.getType().getConstructor(String.class).newInstance(obj.toString());
                            }else if(Date.class.getName().equals(obj.getClass().getName())){
                                //时间
                                SimpleDateFormat sdf = new SimpleDateFormat("yyyy-MM-dd");
                                obj = sdf.parse(obj.toString());
                            }else if(Double.class.getName().equals(obj.getClass().getName())){
                                //浮点型
                            }else if(String.class.getName().equals(obj.getClass().getName())){
                                //字符串
                            }

                            setMethod.invoke(item, obj);
                            //放入最终结果集
                        }
                    }
                }

                resultList.add((T)item);
            }

        } catch (SQLException e) {
            e.printStackTrace();
        } catch (NoSuchMethodException e) {
            e.printStackTrace();
        } catch (SecurityException e) {
            e.printStackTrace();
        } catch (IllegalAccessException e) {
            e.printStackTrace();
        } catch (IllegalArgumentException e) {
            e.printStackTrace();
        } catch (InvocationTargetException e) {
            e.printStackTrace();
        } catch (InstantiationException e) {
            e.printStackTrace();
        } catch (ParseException e) {
            e.printStackTrace();
        } finally{
            super.closeConnection();
        }
        return resultList;
    }

    public <T> T queryById(Integer id, Class<?> cla) {
        // TODO Auto-generated method stub
        return null;
    }

    public <T> int insert(T t) {
        // TODO Auto-generated method stub
        return 0;
    }

    public <T> int insertList(List<T> ts) {
        // TODO Auto-generated method stub
        return 0;
    }

    public <T> int update(T t) {
        // TODO Auto-generated method stub
        return 0;
    }

    public <T> int deleteById(Integer id, Class<?> cla) {
        // TODO Auto-generated method stub
        return 0;
    }


}
