<%--
  Created by IntelliJ IDEA.
  User: chengsanban
  Date: 2020/6/3
  Time: 3:12 PM
  To change this template use File | Settings | File Templates.
--%>
<%@page isELIgnored="false" %>
<%@ page contentType="text/html;charset=UTF-8" language="java" pageEncoding="UTF-8" %>
<%@taglib prefix="c" uri="http://java.sun.com/jsp/jstl/core" %>
<html>
<head>
    <title>bookList</title>
    <style type="text/css">
        table{
            width: 100%;
            border-collapse: collapse;
        }
        table td{
            border-color: #000000;
            border-bottom-width: 1px;
            border-style: solid;
        }
    </style>
</head>
<body>
<table>
    <tr>
        <td colspan="5">
            <a href="insertOrUpdateBook.jsp">新增书本</a>
        </td>
    </tr>
    <tr>
        <td>ID</td>
        <td>书名</td>
        <td>作者</td>
        <td>创建时间</td>
        <td>操作</td>
    </tr>
    <c:forEach var="book" items="${booklist}">
        <tr>
            <td>${book.id}</td>
            <td>《${book.name}》</td>
            <td>${book.author}</td>
            <td>${book.createTime}</td>
            <td>
                <a href="toUpdate.do?id=${book.id}">修改</a>
                <a href="del.do?id=${book.id}">删除</a>
            </td>
        </tr>
    </c:forEach>
    <tr>
        <td colspan="5">
            <a href="tolist.do?curPage=1">首页</a>
            <a href="tolist.do?curPage=${(page.curPage == 1 ? 1 : page.curPage -1)}">上一页</a>
            <span>当前${page.curPage}页/总${page.pageCount}页</span>
            <a href="tolist.do?curPage=${(page.curPage == page.pageCount ? page.pageCount : page.curPage + 1)}">下一页</a>
            <a href="tolist.do?curPage=${page.pageCount}">尾页</a>
        </td>
    </tr>
</table>
</body>
</html>
