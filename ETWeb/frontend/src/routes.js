import React from 'react';
import { BrowserRouter as Router, Switch, Route, Redirect, useRouteMatch, useLocation } from 'react-router-dom';
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

const DashboardRouter = props => {
    const match = useRouteMatch();
    return (
        <Switch>
            <Route exact path={`${match.url}`}>
                <Dashboard.Profile user={props.user}/>
            </Route>
            <Route exact path={`${match.url}/profile`}>
                <Dashboard.Profile user={props.user}/>
            </Route>
            {props.user.is_staff && (
                <Switch>
                    <Route exact path={`${match.url}/projects`}>
                        <Dashboard.MyProjects user={props.user}/>
                    </Route>
                    {/*<Route exact path={`${match.url}/projects/:id`} component={(p)=>{console.log(p); return <a></a>}}>*/}
                    <Route exact path={`${match.url}/projects/:id`}
                           component={({match}) => <Dashboard.ProjectDetail match={match} user={props.user}/>}/>
                </Switch>)
            }
            <Route>
                <Redirect to="/not-found"/>
            </Route>
        </Switch>
    );
};

export default BaseRouter;
export {
    BaseRouter,
    DashboardRouter
}