import React from 'react';
import axios from 'axios';
import * as projectsService from '../../services/projectsService';
import { FormGroup, Alert } from 'reactstrap';
import { Redirect } from 'react-router-dom';

export default class PasswordResetConfirmForm extends React.Component {
    constructor(props) {
        super(props);
        this.state = {
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
            this.setState({successMessage: `${response.data.detail} Redirecting...`});
        } catch (error) {
            console.log(error.message);
            console.log(error.response.data);
            const {data} = error.response;
            if (data !== undefined) {
                this.setState({tokenError: data.token.join(" ")});
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
        const { invitationResolved, successMessage, errors } = this.state;
        return (
                <React.Fragment>
                    <FormGroup>
                        <Alert color="success" isOpen={successMessage !== undefined}>
                            {successMessage}
                        </Alert>
                        <Alert color="danger" isOpen={errors.token !== undefined}>
                            {errors.token}
                        </Alert>
                    </FormGroup>
                    {invitationResolved && (
                        <Redirect to="/"/>
                    )}
                </React.Fragment>
            );
        }
    }
}
