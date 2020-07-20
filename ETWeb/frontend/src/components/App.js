import React, { Component } from 'react';
import BaseRouter from '../routes';
import Layout from '../containers/Layout'
import 'bootstrap/dist/css/bootstrap.css';
import { saveAuthToken, getAuthToken, logoutUser } from "../services/authService";
import '../App.css';

class App extends Component {
    constructor(props) {
        super(props);
        this.state = {
            user: {
                token: getAuthToken()
            }
        };
    }

    handleLogin = (token, remember=false) => {
        saveAuthToken(token, remember);
        this.setState({
            user: {
                token: token
            }
        });
    };

    handleLogout = () => {
        logoutUser();
        this.setState({
            user: {
                token: null
            }
        });
    };

    render() {
        console.log(this.state.user, 'user from app');
        const { user } = this.state;
        return (
            <React.Fragment>
                <Layout user={user}>
                    <BaseRouter
                        user={user}
                        onLogin={this.handleLogin}
                        onLogout={this.handleLogout}
                    />
                </Layout>
            </React.Fragment>
        );
    }
}

export default App;