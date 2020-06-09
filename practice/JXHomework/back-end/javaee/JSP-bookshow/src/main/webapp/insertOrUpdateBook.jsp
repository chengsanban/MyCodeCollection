<%--
  Created by IntelliJ IDEA.
  User: chengsanban
  Date: 2020/6/3
  Time: 5:55 PM
  To change this template use File | Settings | File Templates.
--%>
<%@page isELIgnored="false" %>
<%@ page contentType="text/html;charset=UTF-8" language="java" %>
<!DOCTYPE html>
<html>
<head>
    <meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
    <title>bookList</title>
    <style type="text/css">
        table{
            width: 100%;
            border-collapse: collapse;
        }
        table td {
            border-color: #000000;
            border-width: 1px;
            border-style: solid;
        }
    </style>
</head>
<body>
<form action="insertOrUpdate.do" method="post">
    <input type="hidden" name="id" value="${book.id }"/>
    <table>
        <tr>
            <td>书名：</td>
            <td>
                <input type="text" name="name" value="${book.name }"/>
            </td>
        </tr>
        <tr>
            <td>作者：</td>
            <td>
                <input type="text" name="author" value="${book.author }"/>
            </td>
        </tr>
        <tr>
            <td colspan="2">
                <input type="submit" value="提交"/>
            </td>
        </tr>
    </table>
</form>

</body>
</html>
