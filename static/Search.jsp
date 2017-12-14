<%@page contentType="text/html;charset=utf-8"%>
<%@page import="java.sql.*"%>
<%
String Token=request.getParameter("Token");
out.println("您搜索的词条是："+Token);
%>