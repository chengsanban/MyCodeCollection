package com.packaging.entity;

import com.packaging.annt.Column;
import com.packaging.annt.Table;

@Table(value = "dept")
public class Dept {
    @Column(value = "deptno", select = true)
    private Integer deptno;
    @Column(value = "dname", select = true)
    private String dname;
    @Column(value = "loc", select = true)
    private String loc;
    public Integer getDeptno() {
        return deptno;
    }
    public Dept setDeptno(Integer deptno) {
        this.deptno = deptno;
        return this;
    }
    public String getDname() {
        return dname;
    }
    public Dept setDname(String dname) {
        this.dname = dname;
        return this;
    }
    public String getLoc() {
        return loc;
    }
    public Dept setLoc(String loc) {
        this.loc = loc;
        return this;
    }
    @Override
    public String toString() {
        return "Dept [deptno=" + deptno + ", dname=" + dname + ", loc=" + loc + "]";
    }
}

