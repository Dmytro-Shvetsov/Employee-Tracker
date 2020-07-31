import React, { Component, Suspense } from 'react';
import BaseRouter from '../routes';
import Layout from '../containers/Layout'
import {saveAuthToken, getUserAccount, logoutUser, getAuthToken} from "../services/authService";
import 'bootstrap/dist/css/bootstrap.css';
import '../App.css';


class App extends Component {
    constructor(props) {
        super(props);
        this.state = {
            loadingUser: true,
            user: undefined
        };
        this.tryRestoreSession();
    }

    tryRestoreSession = () => {
        const authToken = getAuthToken();
        getUserAccount(authToken)
        .then(res => {
            const user = JSON.parse(res.data);
            this.setState({
                user: user
            });
        })
        .catch(error => {
            console.log(error.response.data);
            if (error.response.status !== 401) {
                console.error("Unexpected server response when retrieving user account.");
            } else {
                console.error("Couldn't restore session.", error.response.statusText);
            }
        })
        .finally(() => { this.setState({loadingUser: false}) });
    };

    handleLogin = (user, remember) => {
        saveAuthToken(user.token, remember);
        this.setState({
            user: user
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
        if (this.state.loadingUser) {
            return <div>Loading...</div>
        }
        const { user } = this.state;
        console.log(user, 'user from app');
        return (
            <React.Fragment>
                <Suspense>
                    <Layout user={user}>
                        <BaseRouter
                            user={user}
                            onLogin={this.handleLogin}
                            onLogout={this.handleLogout}
                        />
                    </Layout>

                </Suspense>
            </React.Fragment>
        );
    }
}

export default App;