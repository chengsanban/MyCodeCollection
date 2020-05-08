package com.packaging.DAO.imp;

import java.sql.Connection;
import java.sql.PreparedStatement;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.util.LinkedList;
import java.util.List;

import com.packaging.DAO.ScottDao;
import com.packaging.DAO.base.BaseDao;
import com.packaging.entity.Dept;

public class ScottDaoImpl extends BaseDao implements ScottDao{

    public List<Dept> queryDeptByPar(Dept dept) {
        try {
            Connection conn = super.getConnection();
            StringBuilder sql = new StringBuilder();
            sql.append("select deptno , dname , loc from dept where 1=1 ");
            //动态组装sql
            if(dept!=null){
                if(dept.getDeptno()!=null){
                    sql.append(" AND deptno = ? ");
                }
                if(dept.getDname()!=null){
                    sql.append(" AND dname like ? ");
                }
                if(dept.getLoc()!=null){
                    sql.append(" AND loc like ? ");
                }
            }

            PreparedStatement ps = conn.prepareStatement(sql.toString());
            int index = 0;
            //动态给占位符赋值
            if(dept!=null){
                if(dept.getDeptno()!=null){
                    ps.setObject(++index, dept.getDeptno());
                }
                if(dept.getDname()!=null){
                    ps.setObject(++index, "%"+dept.getDname()+"%");
                }
                if(dept.getLoc()!=null){
                    ps.setObject(++index, "%"+dept.getLoc()+"%");
                }
            }

            //获取结果集
            ResultSet result = ps.executeQuery();

            LinkedList<Dept> list = new LinkedList<Dept>();

            while(result.next()){
                Dept d = new Dept();
                d.setDeptno(result.getInt("deptno"));
                d.setDname(result.getString("dname"));
                d.setLoc(result.getString("loc"));
                list.add(d);
            }
            return list;

        } catch (SQLException e) {
            e.printStackTrace();
        } finally{
            super.closeConnection();
        }
        return null;
    }

}
