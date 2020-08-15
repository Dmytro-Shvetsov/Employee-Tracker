import React from 'react';
import { Switch, Route } from 'react-router-dom';
import * as Pages from './components/Pages/index'
import * as Auth from './components/Auth/index'
import * as Dashboard from './components/Dashboard/index'

const BaseRouter = props => {
    return (
        <React.Fragment>
            <Switch>
                {/* Main pages routes */}
                <Route exact path="/" component={Pages.Home}/>
                <Route exact path="/features" component={Pages.Home}/>
                {/*<Route exact path="/idea" component={Home}/>*/}
                {/*<Route exact path="/benefits" component={Home}/>*/}
                {/*<Route exact path="/contact" component={Home}/>*/}

                {/* Auth component group routes */}
                <Route exact path="/register">
                    <Auth.RegisterForm onLogin={props.onLogin}/>
                </Route>
                <Route exact path="/login">
                    <Auth.LoginForm onLogin={props.onLogin}/>
                </Route>
                <Route exact path="/logout">
                    <Auth.Logout onLogout={props.onLogout}/>
                </Route>
                <Route
                    exact path={`/activate/:uidb64([0-9A-Za-z_\\-]+)/:token([0-9A-Za-z]{1,13}-[0-9A-Za-z]{1,20})`}
                    component={({match}) => <Auth.AccountActivation match={match}/>}
                />

                {/*
                  * Dashboard component group routes.
                  * No exact keyword to handle variations with DashboardRouter
                  */}
                <Route path="/dashboard">
                    <Dashboard.Dashboard user={props.user} />
                </Route>

                {/* Unexpected routes */}
                <Route component={Pages.NotFound}/>
            </Switch>
        </React.Fragment>
    );
};

export default BaseRouter;
export {
    BaseRouter,
}