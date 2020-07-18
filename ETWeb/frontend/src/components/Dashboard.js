import React, { Component } from 'react'
// import {  UploadOutlined, UserOutlined, VideoCameraOutlined  } from '@ant-design/icons';
import { Route, NavLink } from 'react-router-dom';
import Home from "./Pages/Home";


const DashboardRouter = props => {
    return (
        <React.Fragment>
            <Route exact path="/dashboard/projects" component={Home}/>
            <Route exact path="/logout" component={Home}/>
        </React.Fragment>
    );
};

const Dashboard = props => {
    const children = props.children;
    return (
        <Layout>
            <Sider
                breakpoint="lg"
                collapsedWidth="0"
                onBreakpoint={broken => {
                    console.log(broken);
                }}
                onCollapse={(collapsed, type) => {
                    console.log(collapsed, type);
                }}
            >
                <div className="logo"/>
                <Menu theme="dark" mode="inline" defaultSelectedKeys={['4']}>
                    <Menu.Item key="1" icon={<UserOutlined/>}>
                        <Link to="/dashboard/projects/">My projects</Link>
                    </Menu.Item>
                    <Menu.Item key="2" icon={<VideoCameraOutlined/>}>
                        <Link to="/dashboard/activity/">Recent activity</Link>
                    </Menu.Item>
                    <Menu.Item key="3" icon={<UploadOutlined/>}>
                        <Link to="/dashboard/activity/">whatever</Link>
                    </Menu.Item>
                    <Menu.Item key="4" icon={<UserOutlined/>}>
                        <Link to="/dashboard/activity/">whatever2</Link>
                    </Menu.Item>
                </Menu>
            </Sider>
            <Content>
                <DashboardRouter />
            </Content>
        </Layout>
    );
};

export default Dashboard;