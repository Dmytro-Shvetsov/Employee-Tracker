import React from 'react';
import * as projects from '../../services/projectsService'
import { Spinner } from 'reactstrap'

export default class MyProjects extends React.Component {
    constructor(props) {
        super(props);
        this.state = {
            user: props.user,
            projects: undefined
        }
    }

    loadProjects = () => {
        const { user } = this.state;
        projects.loadProjectList(user)
                .then(res => {
                    console.log("Successful projects loading");
                    const data = JSON.parse(res.data);
                    this.setState({
                        projects: data
                    });
                })
                .catch(error => {
                    console.error(error.response);
                    console.error(error.response.data);
                })
    };

    componentDidMount() {
        this.loadProjects();
    }

    render() {
        const { projects } = this.state;
        if (projects === undefined) {
            return <Spinner/>
        }
        const { user } = this.state;
        return (
            <div className="row d-flex justify-content-center">
                <h3>Projects for user {user.token}</h3>
            </div>
        );
    }
};