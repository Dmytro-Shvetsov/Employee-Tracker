import React from 'react';
import { Route } from 'react-router-dom';
import RegisterForm from './components/Auth/RegisterForm'
// import Dashboard from './components/Dashboard'

import Home from './components/Pages/Home'

const BaseRouter = () => (
    <React.Fragment>
        <Route exact path="/" component={Home}/>
        <Route exact path="/features" component={Home}/>
        {/*<Route exact path="/idea" component={Home}/>*/}
        {/*<Route exact path="/benefits" component={Home}/>*/}
        {/*<Route exact path="/contact" component={Home}/>*/}
        <Route exact path="/auth/register" component={RegisterForm}/>
        {/*<Route exact path="/auth/login" component={Home}/>*/}
        {/*<Route exact path="/auth/logout" component={Home}/>*/}
    </React.Fragment>
);

export default BaseRouter;