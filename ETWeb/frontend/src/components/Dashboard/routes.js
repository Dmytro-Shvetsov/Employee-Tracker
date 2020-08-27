import * as Dashboard from "./index";
import { NotFound } from '../Pages/index';
import { Switch, Route, Redirect, useRouteMatch } from 'react-router-dom';
import React from "react";

const UserActivityLogsRouter = props => {
    const match = useRouteMatch();
    console.log(match);
    const { user } = props;
    return (
        <Switch>
            <Route exact path={`${match.url}`}>
                <Dashboard.Profile user={ user }/>
            </Route>
            <Route>
                <Redirect to="/not-found"/>
            </Route>
        </Switch>
    );
};

const DashboardRouter = props => {
    const match = useRouteMatch();
    const { user } = props;

    return (
        <Switch>
            <Route exact path={`${match.url}`}>
                <Dashboard.Profile user={user}/>
            </Route>
            <Route exact path={`${match.url}/profile`}>
                <Dashboard.Profile user={user}/>
            </Route>
            <Route exact path={`${match.url}/account`}>
                <Dashboard.Account user={user}/>
            </Route>
            {props.user.is_staff && (
                    <Switch>
                        <Route exact path={`${match.url}/projects`}>
                            <Dashboard.MyProjects user={user}/>
                        </Route>
                        <Route exact path={`${match.url}/project/:id`}
                               component={({ match }) => <Dashboard.ProjectDetail match={match} user={user}/>}
                        />
                        <Route exact path={`${match.url}/activity-logs/user/:id`}
                               component={({ match }) => <Dashboard.UserActivityLogs match={match} user={user}/>}
                        />
                       <Route component={NotFound}/>
                     </Switch>
                )
            }
            <Route component={NotFound}/>
        </Switch>
    );
};

export {
    DashboardRouter,
    UserActivityLogsRouter
}
