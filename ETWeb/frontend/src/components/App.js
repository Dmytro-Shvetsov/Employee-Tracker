import React, { Component } from 'react';
import BaseRouter from '../routes';
import Layout from '../containers/Layout'
import NavBar from "../components/NavBar";
import 'bootstrap/dist/css/bootstrap.css';
import '../App.css';

class App extends Component {
    constructor(props) {
        super(props);
        this.state = {
            user: {
                id: 1,
                username:'John Doe'
            }
        };
    }
    render() {
        return (
            <React.Fragment>
                <Layout>
                    <BaseRouter user={this.state.user}/>
                </Layout>
                {/*{this.state.user ? <DashboardLayout user={this.user}/> : <DefaultLayout/>}*/}
            </React.Fragment>
        );
    }
}

export default App;