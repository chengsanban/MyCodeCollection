package com.chengsanban.mvc.entity;

public class ClassLoader {
    private String name;
    private String clazz;
    private ClassLoader fatherLoader;

    public String getName() {
        return name;
    }

    public void setName(String name) {
        this.name = name;
    }

    public String getClazz() {
        return clazz;
    }

    public void setClazz(String clazz) {
        this.clazz = clazz;
    }

    public ClassLoader getFatherLoader() {
        return fatherLoader;
    }

    public void setFatherLoader(ClassLoader fatherLoader) {
        this.fatherLoader = fatherLoader;
    }
}
