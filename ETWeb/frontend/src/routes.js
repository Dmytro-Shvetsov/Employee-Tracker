import React from 'react';
import { Switch, Route, Redirect, useRouteMatch } from 'react-router-dom';
import { userLoggedIn } from './services/authService';
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
                <Route exact path="/register"
                       component={() => <Auth.RegisterForm onLogin={props.onLogin}/>}/>
                <Route exact path="/login" component={() => <Auth.LoginForm onLogin={props.onLogin}/>}/>
                <Route exact path="/logout" component={() => <Auth.Logout onLogout={props.onLogout}/>}/>
                {/*
                  * Dashboard component group routes.
                  * No exact keyword to handle variations with DashboardRouter
                  */}
                <Route path="/dashboard" component={() => <Dashboard.Dashboard user={props.user} />}/>

                {/* Unexpected routes */}
                <Route component={Pages.NotFound}/>
            </Switch>
        </React.Fragment>
    );
};

const DashboardRouter = props => {
    const routeMatch = useRouteMatch();
    const { user } = props;

    return (
        <React.Fragment>
            <Switch>
                <Route exact path={`${routeMatch.url}/`} component={() => <Dashboard.Profile user={props.user}/>}/>
                <Route exact path={`${routeMatch.url}/profile`} component={() => <Dashboard.Profile user={props.user}/>}/>
                {user.is_staff && (<Route
                                        exact path={`${routeMatch.url}/projects`}
                                        component={() => <Dashboard.MyProjects user={props.user}/>}
                                   />)}
                <Route component={() => <Redirect to="/not-found"/>}/>
            </Switch>
        </React.Fragment>
    );
};

export default BaseRouter;
export {
    BaseRouter,
    DashboardRouter
}