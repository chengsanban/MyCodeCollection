package com.chengsanban.mvc.loader;

import com.chengsanban.mvc.entity.ContextApplication;
import com.chengsanban.mvc.exception.SpringFrameworkLoadListenerException;
import org.dom4j.Document;
import org.dom4j.Element;
import org.dom4j.io.SAXReader;

import java.io.File;
import java.lang.reflect.Field;
import java.lang.reflect.Method;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

public abstract class ContextLoader {
    private Document document;
    protected static Map<String, ContextApplication> applicationContext =
            new HashMap<String, ContextApplication>();
    //为web工程
    public void init(String webPath){
        try {
            checkPath(webPath);
            build();
            int i = 1;
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    //为java工程
    public void init(){

    }

    //检测文件是否存在
    private void checkPath(String webPath) throws Exception{
        webPath += "/WEB-INF/application.xml";
        File file = new File(webPath);
        if(file.exists()){
            SAXReader sax = new SAXReader();
            document = sax.read(file);
        }else{
            throw new Exception(webPath + "不存在");
        }
    }

    private void build() throws Exception{
        Element root = this.document.getRootElement();
        List<Element> beans = root.elements("bean");

        for(Element bean : beans){
            String name = bean.attributeValue("name");
            String clazz = bean.attributeValue("class");
            //获取类实例，并用一个类包装
            System.out.println(clazz);
            Object pObj = Class.forName(clazz).newInstance();
            ContextApplication pCA = new ContextApplication();
            pCA.setObj(pObj);

            //获取注入的内容
            List<Element> opts = bean.elements("opt");
            for(Element opt : opts){
                String cname = opt.attributeValue("name");
                String ref = opt.attributeValue("ref");
                ContextApplication CA = this.applicationContext.get(ref);
                if(CA == null){//注入对象没有找到
                    throw new SpringFrameworkLoadListenerException(ref+":没找到！");
                }else{
                    Object cObj = CA.getObj(); //需要注入的对象
                    //获取要注入的属性
                    Field field = pObj.getClass().getDeclaredField(cname);
                    //获取set方法的方法名
                    String methodName = "set" + String.valueOf(cname.charAt(0)).toUpperCase()
                            + cname.substring(1);
                    //获取set方法
                    Method method = pObj.getClass().getMethod(methodName, field.getType());
                    //执行完成注入
                    method.invoke(pObj, cObj);
                }
            }
            //加入依赖关系集合
            this.applicationContext.put(name, pCA);
        }
    }
}
