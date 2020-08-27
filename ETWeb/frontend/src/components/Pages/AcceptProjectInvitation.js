import React from 'react';
import axios from 'axios';
import * as projectsService from '../../services/projectsService';
import { FormGroup, Alert } from 'reactstrap';
import { Redirect } from 'react-router-dom';

export default class AcceptProjectInvitation extends React.Component {
    constructor(props) {
        super(props);
        this.state = {
            user: props.user || props.location.state && props.location.state.user,
            token: undefined,
            successMessage: undefined,
            tokenError: undefined,
            invitationResolved: false
        };
        this.reqSource = undefined;
        this._isMounted = false;
    }

    setState = (...args) => {
        if (this._isMounted) {
            super.setState(...args);
        }
    };

    acceptInvitation = async token => {
        await this.cancelPreviousRequests();
        try {
            const response = await projectsService.acceptProjectInvitation(token, this.reqSource.token);
            // console.log(response.data);
            this.setState({successMessage: response.data.detail + " Redirecting..."});
        } catch (error) {
            console.log(error.message);
            const {data} = error.response;
            if (data !== undefined) {
                this.setState({tokenError: data.token.join(" ") +  " Redirecting..."});
            }
        }
        // redirect in 4 seconds
        this.timeout = setTimeout(() => {
            this.setState({invitationResolved: true});
        }, 3000);
    };

    cancelPreviousRequests = async () => {
        if (this.reqSource) {
            this.reqSource.cancel();
        }
        this.reqSource = axios.CancelToken.source();
    };

    static getDerivedStateFromProps(props, state) {
        if (props.user !== state.page) {
          return {
            user: props.user,
          };
        }

        // Return null to indicate no change to state.
        return null;
    }

    async componentDidMount() {
        this._isMounted = true;
        await this.acceptInvitation(this.props.match.params.token);
    }

    async componentWillUnmount() {
        this._isMounted = false;
        await this.cancelPreviousRequests();
        if (this.timeout !== undefined) {
            clearTimeout(this.timeout);
        }
    }

    render() {
        const {user, invitationResolved, successMessage, tokenError} = this.state;
        return (
            <React.Fragment>
                {invitationResolved && (
                    <Redirect to="/"/>
                )}
                <FormGroup>
                    <Alert color="success text-center" isOpen={successMessage !== undefined}>
                        {successMessage}
                    </Alert>
                    <Alert color="danger text-center" isOpen={tokenError !== undefined}>
                        {tokenError}
                    </Alert>
                </FormGroup>
                {(user === undefined || user === null) && (
                    <Redirect to={{
                        pathname:"/login",
                        state: {next:this.props.match.url}
                    }}/>
                )}
            </React.Fragment>
        );
    }
}
