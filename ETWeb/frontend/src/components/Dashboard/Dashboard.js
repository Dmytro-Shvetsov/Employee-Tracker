import React, { Component } from 'react'
import { userLoggedIn } from '../../services/authService'
// import {  UploadOutlined, UserOutlined, VideoCameraOutlined  } from '@ant-design/icons';
import { Redirect } from 'react-router-dom';
import { DashboardRouter, BreadCrumb } from '../../routes';
import { DashboardNavBar } from '../NavBar';
import { NotFound } from '../Pages/index';


export default class Dashboard extends React.Component{
    constructor(props) {
        super(props);
        this.state = {
            user: props.user
        };
    }
    render() {
        const { user } = this.state;
        if (!userLoggedIn(user)) {
            console.warn("Not logged in user accesses dashboard page. Redirectin");
            return <NotFound/>
        }

        return (
            <React.Fragment>
                <div className="row" id="dashboard">
                    <div className="col-3" id="dashboard-navbar">
                        <DashboardNavBar user={user} />
                    </div>
                    <div className="col-9" id="dashboard-content">
                        <DashboardRouter user={user}/>
                    </div>
                </div>
            </React.Fragment>
        );
    }
}