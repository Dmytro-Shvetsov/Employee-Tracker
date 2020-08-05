import React from 'react'
import { userLoggedIn } from '../../services/authService'
// import {  UploadOutlined, UserOutlined, VideoCameraOutlined  } from '@ant-design/icons';
import { DashboardRouter } from '../../routes';
import { DashboardNavBar } from '../NavBar';


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
            console.warn("Not logged in user accesses dashboard page.");
            window.location.replace("/login");
            return;
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