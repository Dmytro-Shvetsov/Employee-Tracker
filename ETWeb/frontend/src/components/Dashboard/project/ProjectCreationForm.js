import React from 'react';
import axios from 'axios';
import { Form, FormGroup, Alert } from 'reactstrap';
import {Modal, Input} from '../../common';
import * as projectsService from "../../../services/projectsService";


export default class ProjectCreationForm extends React.Component {
    constructor(props) {
        super(props);
        this.state = {
            user: props.user,
            modal: false,
            data: {
                name: '',
                description: '',
                members: [],
                projectCreated: false,
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

    handleInputChange = event => {
        const target = event.target;
        const name = target.name;

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

    // todo
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
        const { user, data } = this.state;
        try {
            const response = await projectsService.createNewProject({...data, user: {...user}}, this.reqSource.token);
            console.log("Successfully created project");
            const {id} = JSON.parse(response.data);
            this.setState({
                modal: false
            });
            this.props.onProjectCreated(id)
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
        const { modal, errors } = this.state;
        return (
            <Modal
                modalTitle="New project creation"
                triggerBtnLabel="New project"
                actionBtnLabel="Create"
                onToggle={this.handleToggle}
                modal={modal}
                onAction={this.handleSubmit}
                className="d-flex justify-content-end"
            >
                <Form className="" autoComplete="on">
                    <Input
                        name="name"
                        labelText="Name"
                        error={errors.name}
                        onChange={this.handleInputChange}
                    />
                    <Input
                        name="description"
                        labelText="Brief description"
                        error={errors.description}
                        onChange={this.handleInputChange}
                        type="textarea"
                        required={false}
                    />
                    <FormGroup>
                        <Alert color="danger" isOpen={errors.non_field_errors !== undefined}>
                            {errors.non_field_errors}
                        </Alert>
                    </FormGroup>
                </Form>
            </Modal>
        );
    }
}