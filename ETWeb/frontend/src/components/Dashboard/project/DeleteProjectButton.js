import React from 'react';
import axios from 'axios';
import { FormGroup, Alert } from 'reactstrap';
import {Modal} from '../../common';
import * as projectsService from "../../../services/projectsService";


export default class DeleteProjectButton extends React.Component {
    constructor(props) {
        super(props);
        this.state = {
            windowModalOpen: false,
            projectId: props.projectId,
            errorDetail: undefined,
        };
        this.reqSource = undefined;
        this._isMounted = false;
    }

    setState = (...args) => {
        if (this._isMounted) {
            super.setState(...args);
        }
    };

    windowModalToggle = () => {
        this.setState({
            windowModalOpen: !this.state.windowModalOpen,
            // reset other fields
            errors: {}
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
        console.log("Trying to delete project.");
        await this.cancelPreviousRequests();
        const { projectId } = this.state;
        try {
            const response = await projectsService.deleteProject(projectId, this.reqSource.token);
            this.setState({
                errors: {}
            });
            this.windowModalToggle();
            this.props.onProjectDeleted();
        } catch (error) {
            console.log(error.message);
            const {data} = error.response;
            const errorDetail = data !== undefined ? data.detail : "Unexpected error occurred.";
            this.setState({errorDetail});
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
        const {windowModalOpen, errorDetail} = this.state;
        // console.log(matchesDropdownOpen);
        return (
            <Modal
                triggerBtnLabel="Delete this project"
                modalTitle="Project delete confirmation"
                triggerBtnColor="danger"
                actionBtnLabel="Yes"
                onAction={this.handleSubmit}
                onToggle={this.windowModalToggle}
                modal={windowModalOpen}
            >
                <span>
                    Are you sure you would like to delete this project? You won't be able to access any
                    members information afterwards.
                </span>
                <FormGroup className="mt-2">
                    <Alert color="danger" isOpen={errorDetail !== undefined}>
                        {errorDetail}
                    </Alert>
                </FormGroup>
            </Modal>
        );
    }
}