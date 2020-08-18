import React from 'react';
import axios from 'axios';
import { Form, FormGroup, Alert } from 'reactstrap';
import {Modal, Input} from '../../common';
import * as projectsService from "../../../services/projectsService";


export default class AddMembersForm extends React.Component {
    constructor(props) {
        super(props);
        this.state = {
            user: props.user,
            modal: false,
            data: {
                usernamePattern: '',
                description: '',
                members: [],
                membersAdded: false,
            },
            errors: {},
        };
        this.reqSource = undefined;
        this._isMounted = false;
    }

    setState = (...args) => {
        if (this._isMounted) {
            super.setState(...args);
        }
    };

    handleUsernameChange = async event => {
        const target = event.target;
        await this.cancelPreviousRequests();
        try {
            const response = await projectsService.findMembersByUsername(
                {username: target.value}, this.reqSource.token
            );
            const {data} = this.state;

            this.setState({
                ...data,
                members: response.members
            });
        }
        this.setState({
            data: { ...this.state.data, [name]: target.value },
            errors: { ...this.state.errors, [name]: undefined },
        });
    };

    handleToggle = () => {
        const { modal } = this.state;
        this.setState({
            modal: !modal
        })
    };

    handleMemberAdd = event => {
        console.log('adding member');
        this.setState({
            // rememberMe: !this.state.rememberMe,
        });
    };

    cancelPreviousRequests = async () => {
        if (this.reqSource) {
            this.reqSource.cancel();
        }
        this.reqSource = axios.CancelToken.source();
    };

    handleSubmit = async event => {
        event.preventDefault();
        console.log("Trying to create new project");
        await this.cancelPreviousRequests();
        const { data } = this.state;
        try {

        } catch (error) {
            if (error.response.status === 400) {
                const fieldErrors = error.response.data;
                Object.keys(fieldErrors).map((fieldName) => {
                    fieldErrors[fieldName] = fieldErrors[fieldName].join(" ");
                    console.log(fieldErrors[fieldName]);
                });

                this.setState({
                    errors: fieldErrors
                });
            }
        }
    };

    componentDidMount() {
        this._isMounted = true;
    }

    async componentWillUnmount() {
        this._isMounted = false;
        await this.cancelPreviousRequests();
    }

    render() {
        const {modal, errors} = this.state;
        return (
            <Modal
                // className="bg-warning"
                triggerBtnLabel="Add new members"
                triggerBtnColor="success"
                onAction={() => {
                    alert(1)
                }}
                actionBtnLabel={"Add"}
                onToggle={this.handleAddMembersToggle}
                modal={addMembersModal}
            >
            </Modal>
        );
    }
}